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

Welke diagrammen je moet bijwerken zodat ze precies met de code overeenkomen,
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
Staat het beeld op zijn kop, haal dan `#define DRAAI_180` weg.

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

Het meetprogramma gebruikt dezelfde Mpu6050, BalansFilter en MotorDriver als
de robotfirmware. Het loopt vanzelf door vijf stappen en zegt in de monitor wat
je moet doen:

1. rechtop en stil: resterende gyro-offset en ruis;
2. kantelen: richting van de kantelas;
3. valtest: hoe snel de robot omvalt (omega);
4. motorrichting (wielen vrij);
5. richting van de regeling (wielen vrij, alleen P).

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
(massa, zwaartepunt, wielen, motoren). `PID_KP` = 80, `PID_KI` = 0 en
`PID_KD` = 5 zijn startwaarden met het goede teken, afgeleid uit de valtest en
het 3D-model (zie de uitleg in `Ontwerpparameters.hpp`).

**Bij het opstarten:** houd de robot 2 seconden stil rechtop, in de stand
waarin hij in balans is. Dat is het nulpunt (UC-S3-01). Pas daarna gaat hij
balanceren.

1. **Richtingstest (wielen vrij).** Houd de robot vast, laat hem kalibreren en
   kantel hem een paar graden voorover. Beide wielen moeten **vooruit**
   draaien (richting scherm). Achterover moeten ze achteruit draaien. Gaat het
   andersom, verander dan niet zomaar de tekens maar zoek eerst uit waarom.
2. **Alleen P.** Zet `PID_KD` = 0. Verhoog `PID_KP` in stappen van 20 tot de
   robot zichzelf terugduwt en snel heen en weer gaat slingeren.
3. **D erbij.** Verhoog `PID_KD` in stappen van 0,5 tot het slingeren
   gedempt is. Gaat hij snel trillen of zoemen, dan is `PID_KD` te hoog.
4. **I als laatste.** Rijdt hij langzaam steeds één kant op, zet dan
   `PID_KI` op 0,5 tot 2.
5. **Trilt hij hard rond rechtop,** zelfs met kleine P? Verlaag dan
   `MOTOR_STARTWAARDE` (bijvoorbeeld naar 550). Een motor die al draait heeft
   minder nodig dan een motor die vanuit stilstand moet starten.

Noteer per proef de waarden en wat je zag; die horen in het ontwikkeldocument
(hoofdstuk Ontwerpparameters).

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
