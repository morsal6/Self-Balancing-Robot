# Firmware zelfbalancerende robot

De firmware bestaat uit twee ESP-IDF 5.4-projecten, één per subsysteem, en een
meetprogramma:

| Map | Chip | Taken (FreeRTOS) |
| :--- | :--- | :--- |
| `esp32s3/` | ESP32-S3 Super Mini | RobotControl, Mpu6050, VerstuurControl |
| `cyd/` | CYD (ESP32-2432S028) | TonenControl, ScoreControl, Touchscreen, Display |
| `test_s3_meting/` | ESP32-S3 Super Mini | Meetprogramma (geen onderdeel van het ontwerp) |

De code is afgeleid van de STD's volgens het hoofdstuk "Van STD naar Code" uit
*Design Like a Robot! (FreeRTOS)*. Per actieve klasse heb je:

- een `enum class State` met de toestanden uit het STD;
- eventbits;
- een constructor die als laatste de taak start met `xTaskCreatePinnedToCore`;
- de STD-interface als publieke functies;
- een `main()` met een `switch (state)`.

De passieve controls (InitControl, BalansControl, BalansverliesControl) volgen
hun activity diagram voor `verwerkMeting()`.

Welke diagrammen bij deze code horen en wat er in die diagrammen is veranderd,
staat in `DIAGRAM-AANPASSINGEN.md`.

## Bouwen en flashen (Windows, ESP-IDF 5.4)

Pak de zip uit in een korte map, bijvoorbeeld `C:\robot`. Bij een lang pad
(zoals een map in Downloads) kan de build mislukken omdat Windows-paden te
lang worden.

Open de ESP-IDF 5.4 PowerShell (of CMD) en flash per board:

```powershell
cd C:\robot\esp32s3
idf.py set-target esp32s3
idf.py -p COM5 flash monitor -d
```

```powershell
cd C:\robot\cyd
idf.py set-target esp32
idf.py -p COM6 flash monitor -d
```

- Vervang `COM5`/`COM6` door de poort van het board (Apparaatbeheer).
- `set-target` is alleen de eerste keer in een map nodig. Het maakt de
  `sdkconfig` opnieuw aan uit `sdkconfig.defaults`.
- `flash` bouwt eerst, dus een losse `build` is niet nodig.
- `-d` start de monitor zonder adresvertaling. Zonder `-d` blokkeert Windows
  (Smart App Control) soms `xtensa-esp32-elf-addr2line`.
- Stoppen van de monitor: `Ctrl+]`.

Gebruik je een oude map opnieuw en heb je `sdkconfig.defaults` aangepast?
Verwijder dan eerst `sdkconfig` en de map `build`, of voer `set-target` opnieuw
uit; anders blijven de oude instellingen staan.

De CYD haalt `espressif/esp_lcd_ili9341` op via de component manager
(`cyd/main/idf_component.yml`), net als in het lesmateriaal. Het bestand
`cyd/dependencies.lock` legt de versie vast (1.0.0).

## Structuur (volgens CodeStyle.md)

```text
<project>/
├─ CMakeLists.txt
├─ sdkconfig.defaults
├─ main/main.cpp            alleen objecten aanmaken
└─ components/
   ├─ core/                 Ontwerpparameters.hpp, TryReadQueue.hpp
   ├─ protocol/             Bericht.hpp (identiek in beide projecten)
   ├─ drivers/              boundary-objecten (hardware)
   ├─ app/                  controls en entities
   ├─ ili9341_display/      (cyd) uit TI-S3/software/CYD/Game_sprites
   └─ drawables/            (cyd) uit TI-S3/software/CYD/Game_sprites
```

Ontwerpwaarden en taakparameters staan op één plek:
`components/core/Ontwerpparameters.hpp`. De namen zijn dezelfde als in de
STD's (REGELPERIODE, BALANSVERLIESGRENS, COMM_TIMEOUT, ...).

## Verdeling over de cores

### ESP32-S3

| Core | Taak | Prioriteit | Waarom |
| :--- | :--- | :--- | :--- |
| 1 | RobotControl | 10 | Verwerkt iedere meting direct (PID + motoren) |
| 1 | Mpu6050 | 9 | Meet iedere REGELPERIODE op vaste momenten (`xTaskDelayUntil`) |
| 0 | VerstuurControl | 5 | ESP-NOW, naast de WiFi-taak van ESP-IDF |

Op core 1 draait alleen de regellus. Core 0 doet alle communicatie: daar
draaien ook de WiFi-taak, `esp_timer` en de FreeRTOS-timertaak (Tmr Svc, met
de VerzendTimer). `sdkconfig.defaults` zet die vast op core 0. WiFi wordt
gestart in de constructor van EspNowZender (in `app_main`, core 0). Zo kan
ESP-NOW de taken van de regellus niet onderbreken (NFR-02).

RobotControl heeft een hogere prioriteit dan Mpu6050. Zodra Mpu6050 een meting
meldt, onderbreekt RobotControl hem en verwerkt de meting meteen. Daardoor gaat
er tijdens het regelen geen meting verloren in de queue van lengte 1.

### CYD

| Core | Taak | Prioriteit |
| :--- | :--- | :--- |
| 0 | TonenControl | 5 |
| 0 | ScoreControl | 4 |
| 0 | Touchscreen | 3 |
| 1 | Display | 5 |

Display heeft core 1 voor zichzelf: één `present()` van het hele scherm kost
ruim 40 ms.

## Pinnen (gemeten met het meetprogramma)

ESP32-S3 (`esp32s3/main/main.cpp`):

| Signaal | GPIO | Opmerking |
| :--- | :--- | :--- |
| MPU6050 SDA / SCL | 8 / 9 | adres 0x68, 100 kHz |
| Linkermotor BI / FI | 5 / 4 | BI hoog = vooruit |
| Rechtermotor BI / FI | 7 / 6 | BI hoog = vooruit |

Vooruit is richting de kant met het scherm.

CYD: de pinnen van het scherm komen uit het lesmateriaal. Het touchscreen
gebruikt alleen T_IRQ (GPIO36) van de XPT2046: laag = aangeraakt. Heb je het
2,4 inch-scherm, haal dan `#define DISPLAY28` weg in `Ili9341Display.hpp`.
De spiegeling staat in `setupPanel()` in `Ili9341Display.cpp`. Staan de cijfers
in spiegelbeeld of op hun kop, pas daar `esp_lcd_panel_mirror` aan.

## ESP-NOW

De S3 stuurt rechtstreeks naar het MAC-adres van de CYD (unicast). Dat adres
staat in `CYD_MAC` in `components/protocol/Bericht.hpp`; houd dat bestand in
beide projecten gelijk. De WiFi-hardware van de CYD bevestigt elk bericht. De
S3 logt `CYD bereikbaar` of `CYD niet bereikbaar` zodra dat verandert. Die
bevestiging zegt alleen dat het bericht is aangekomen, niet dat TonenControl
het al verwerkt heeft.

Gebruik je een andere CYD, zet dan zijn MAC-adres in `CYD_MAC`. Het staat bij
het opstarten in de monitor van de CYD. Broadcast (`FF:FF:FF:FF:FF:FF`) werkt
ook, maar dan is er geen ontvangstbevestiging en zegt `CYD bereikbaar` niets
meer.

## Fouten

| Situatie | Wat de firmware doet |
| :--- | :--- |
| MPU6050 niet in te stellen bij het opstarten | Log `MPU6050 instellen mislukt`; alle metingen zijn ongeldig, dus RobotControl gaat naar Fout (UC-S3-05), motoren stil |
| Uitlezen mislukt 3 keer achter elkaar | Ongeldige meting, dus Fout. De log meldt `Uitlezen mislukt` één keer, niet elke 5 ms |
| Losse I2C-storing | Opnieuw lezen (maximaal 3 pogingen); de regeling gaat gewoon door |
| Een taak kan niet starten (te weinig geheugen) | `configASSERT` geeft een melding in de monitor en het board herstart |
| Backbuffer van de CYD niet te reserveren | Log van `Ili9341Display`; er wordt niets getekend |

## Meetgegevens die in de code zitten

| Meting | Uitkomst | Waar in de code |
| :--- | :--- | :--- |
| Kantelas | y-as; voorover positief (+80 graden plat voorover, -53 graden achterover) | `BalansFilter` |
| Meetperiode | 5 ms (200 Hz) haalbaar | `REGELPERIODE` |
| Gyro-offset gy | -3,14 graden/s, ruis 0,69 graden/s | `GY_OFFSET` |
| Gyro-ruis stil in de hand | tot ca. 4,5 graden/s | `STILGRENS` = 10 |
| Valtest | omega 7,1 en 4,8 per seconde | `PID_KP`, `PID_KD` |
| Motorrichting | BI = vooruit, FI = achteruit | pinnen in `main.cpp` |
| Startwaarde motoren | PWM 160 van 255 bij 10 kHz (wielen vrij) | `MOTOR_STARTWAARDE` = 627 van 1000 |

## Meetprogramma (test_s3_meting)

Het meetprogramma gebruikt dezelfde Mpu6050, BalansFilter, PidRegelaar en
MotorDriver als de robotfirmware, met de waarden uit `Ontwerpparameters.hpp`.
Het zegt in de monitor wat je moet doen:

1. rechtop en stil: nulpunt, resterende gyro-offset en ruis;
2. kantelen: richting van de kantelas;
3. valtest: hoe snel de robot omvalt (omega);
4. motorrichting (wielen vrij);
5. richting van de regeling (wielen vrij, vaste uitgang vanaf 3 graden);
6. balanceertest: 3 pogingen van maximaal 60 s (NFR-04) op de vloer, met een logboek
   (tijd, balanswaarde, gy en motorcommando, elke 100 ms). Een leesfout stopt de poging (SENSORFOUT), net als op de robot.

Met `ALLEEN_BALANSTEST = true` (bovenin `main/main.cpp`) doet hij alleen stap 1
en 6. Zet hem op `false` voor alle stappen.

```powershell
cd C:\robot\test_s3_meting
idf.py set-target esp32s3
idf.py -p COM5 flash monitor -d
```

Kopieer aan het eind het blok tussen de streepjeslijnen. Mpu6050 trekt
`GY_OFFSET` al af; stap 1 geeft dus de rest en meteen de nieuwe waarde voor
`GY_OFFSET` (oude waarde + rest). Flash daarna de robotfirmware (`esp32s3`)
opnieuw.

## Hardware tegen storing en wegschieten

- 10 kOhm pull-down van elke ZK5AD-ingang (GPIO 4, 5, 6, 7) naar GND. Tijdens
  een reset of het opstarten van de S3 zweven die pinnen ca. 0,1 s; zonder
  pull-down kunnen de motoren dan vol gaan draaien.
- 100 nF keramisch direct over de aansluitingen van elke motor, en 470 uF of
  meer over de voedingsingang van de ZK5AD.
- GND van ZK5AD en S3 op één punt verbinden; I2C-draden kort en weg van de
  motordraden.

## PID afstellen

Exacte PID-waarden kun je niet uitrekenen zonder volledig model van de robot
(massa, zwaartepunt, wielen, motoren). De huidige waarden zijn `PID_KP` = 80,
`PID_KI` = 200, `PID_KD` = 2 en `FILTER_ALFA` = 0,995. De uitleg staat in
`Ontwerpparameters.hpp`. Kort:

- De TT-motoren met tandwielkast gedragen zich als snelheidsbron. Om scheef
  staan te corrigeren moet de robot blijven versnellen, en dat doet de I-term.
  Zonder I (`PID_KI` = 0) valt hij steeds om of schiet hij weg.
- Bij `FILTER_ALFA` = 0,98 telde het optrekken van de robot te veel mee in de
  hoek. Dan onderschat hij zijn kanteling en rijdt hij weg.

### Snelheidsterugkoppeling

Met alleen de hoek kan de robot niet blijven staan. Het nulpunt is nooit
precies het balanspunt: staat het 1 of 2 graden verkeerd, dan "balanceert" de
regelaar op een hoek waarbij de robot eigenlijk omvalt. Hij blijft dan
versnellen, rijdt steeds harder één kant op (bij deze robot achteruit) en valt.

Daarom kiest `PidRegelaar` eerst een doelhoek en regelt hij daarnaar:

```text
doelhoek  = -(SNELHEID_KP * snelheid + SNELHEID_KI * snelheidIntegraal)
afwijking = balanswaarde - doelhoek
```

Rijdt de robot achteruit, dan wordt de doelhoek positief: hij helt iets
voorover, remt af en rijdt terug. Er zitten geen encoders op de wielen, dus
`snelheid` is het motorcommando door een laagdoorlaatfilter
(`SNELHEID_FILTERTIJD` = 0,3 s), van -1 (vol achteruit) tot 1 (vol vooruit).
`snelheidIntegraal` is een maat voor de afgelegde weg.

Daarvoor moet de snelheid uit het echte motorcommando komen, inclusief de
dode-zonecompensatie (`MOTOR_STARTWAARDE`). Die compensatie zit daarom nu in
`PidRegelaar` en niet meer in `MotorDriver`. `MotorDriver` begrenst alleen nog
de stap per 5 ms.

In een simulatie met het gemeten slingermodel (omega 4,8 tot 7,1 per seconde,
nulpunt tot 3 graden fout) bleef de robot zonder snelheidsterugkoppeling
gemiddeld 3 s staan. Met de waarden hierboven bleef hij in ruim driekwart van
de gevallen 60 s staan (NFR-04), en reed hij meestal minder dan 0,5 m heen en
weer. Een simulatie is geen robot: de balanceertest beslist.

**Bij het opstarten:** houd de robot 2 seconden stil rechtop, in de stand
waarin hij in balans is. Dat is het nulpunt (UC-S3-01). Het gezicht heeft dan
halfopen ogen. Pas daarna gaat hij balanceren. Kalibreer zo recht mogelijk:
hoe groter de fout, hoe verder hij heen en weer rijdt.

Houd de robot niet vast terwijl hij balanceert om te kijken wat de wielen doen:
hij merkt dan dat hij scheef staat en zet de motoren steeds harder aan. De
snelheidsterugkoppeling denkt dan dat hij rijdt; na het loslaten schiet hij
eerst een stukje weg.

1. **Richtingstest (wielen vrij).** Houd de robot vast, laat hem kalibreren en
   kantel hem een paar graden voorover. Beide wielen moeten **vooruit**
   draaien (richting scherm). Achterover moeten ze achteruit draaien. Is het
   andersom, wissel dan de motordraden. Maak de versterkingen niet negatief:
   dan werkt de snelheidsterugkoppeling de verkeerde kant op.
2. **Balanceertest met logboek** (meetprogramma, stap 6). Stuur het blok tussen
   de streepjeslijnen op; daaruit is te zien wat er misgaat.
3. Wat je in het logboek of aan de robot ziet:

| Wat je ziet | Wat je aanpast |
| :--- | :--- |
| Valt meteen om, de wielen reageren te laat of te zwak | `PID_KP` +20 |
| Snel heen en weer trillen of zoemen (meer dan 4 wissels/s) | `PID_KD` -1, daarna `PID_KP` -10 |
| Slingert in minder dan een seconde heen en weer, steeds groter | `PID_KI` -50 of `PID_KD` +1 |
| Rijdt nog steeds weg en valt (gem. u ver van 0) | `SNELHEID_KP` +2; en opnieuw kalibreren, rechter in het balanspunt |
| Rijdt langzaam (een paar seconden) heen en weer, steeds verder | `SNELHEID_KI` -1 |
| Komt na een duw niet terug naar zijn plek | `SNELHEID_KI` +1 |

Pas steeds één waarde tegelijk aan en noteer per proef de waarden en wat je
zag; die horen in het ontwikkeldocument (hoofdstuk Ontwerpparameters).

## Geheugen van de CYD

Met WiFi aan is op de CYD geen aaneengesloten blok van 153.600 bytes vrij
voor een backbuffer van het hele scherm (gemeten bij de start: 139.264 bytes).
Daarom is `Ili9341Display` uit het lesmateriaal op één punt aangepast: de
backbuffer bestaat uit twee helften van 76.800 bytes (boven en onder), en
`present()` stuurt ze na elkaar naar het scherm. De rest van de API
(`drawPixel`, `fillScreen`, `present`, de drawables) is gelijk gebleven. De
wachttijd van 41 ms in `present()` komt uit de lesdriver: zo lang duurt het
versturen van een heel scherm bij 30 MHz SPI.

De constructor van Display roept `lcd.init()` aan, en Display is het eerste
object in `app_main`. Zo worden beide helften gereserveerd voordat WiFi en de
andere taken geheugen nemen.

## Gezicht en drawables

Het tekenen volgt het lesmateriaal (TI-S3, `software/CYD/Game_sprites` en
`LCD_drawables`): `Drawable` met de subklassen `Circle`, `Rectangle`, `Line`
en `Triangle`, en `Ili9341Display` met de tekenprimitieven. Display maakt een
drawable en roept `draw(lcd)` aan; alles komt in de backbuffer en
`lcd.present()` zet het in één keer op het scherm. Display gebruikt alleen
`Circle`, `Rectangle` en `Line`, zoals in het klassediagram.

In de lesdriver waren drie primitieven nog niet af (de opdracht "ESP32 CYD
Drawables"). Die zijn nu ingevuld:

| Primitief | In de lesdriver | Nu |
| :--- | :--- | :--- |
| `drawLine` | Noodoplossing: alleen horizontaal van links naar rechts en verticaal van boven naar beneden | Uit de opdracht "ESP32 CYD Drawables": Bresenham met `drawLineLow` (vlakke lijnen) en `drawLineHigh` (steile lijnen) |
| `drawCircle` | Rand: leeg (TODO) | Uit de opdracht "ESP32 CYD Drawables": midpoint-cirkel; gevuld met horizontale lijnen |
| `drawTriangle` met `fill = true` | Leeg (TODO) | De drie randen, en dan rij voor rij elke pixel die binnen alle drie de randen ligt |

Daardoor ontbraken eerst alle schuine lijnen en cirkelranden: het gevallen
gezicht (kruisjes) was leeg, en het schrikgezicht en het foutgezicht hadden
geen mond.

| Fase (S3) | Gezicht | Wanneer |
| :--- | :--- | :--- |
| KALIBREREN | Halfopen ogen, kleine mond | Bij het opstarten van de S3 (ook na een herstart). Na een val wordt niet opnieuw gekalibreerd: rechtop en 1 s stil zetten is genoeg |
| BALANCEREN | Ronde ogen, pupillen kijken mee met de kanteling, lach | \|balanswaarde\| <= `SCHRIKGRENS` (12 graden) |
| BALANCEREN | Grote ogen, kleine pupillen, O-mond (schrikken) | \|balanswaarde\| > 12 graden; dan staan de motoren ook (bijna) vol open |
| BALANSVERLIES | Kruisjes als ogen, frons | Na een val |
| FOUT | Rode ogen, rechte mond | Sensorfout |
| (geen bericht) | Grijze dichte ogen, drie puntjes | Langer dan `COMM_TIMEOUT` (500 ms) geen geldig bericht |

Wil je het schrikgezicht pas later zien, verhoog dan `SCHRIKGRENS` in
`cyd/components/core/Ontwerpparameters.hpp`.
