# Firmware zelfbalancerende robot

De firmware bestaat uit twee ESP-IDF 5.4-projecten, één per subsysteem:

| Map | Chip | Taken (FreeRTOS) |
| :--- | :--- | :--- |
| `esp32s3/` | ESP32-S3 Super Mini | RobotControl, Mpu6050, VerstuurControl |
| `cyd/` | CYD (ESP32-2432S028) | TonenControl, ScoreControl, Touchscreen, Display |

De code is afgeleid van de STD's volgens het hoofdstuk "Van STD naar Code" uit
*Design Like a Robot! (FreeRTOS)*. Per actieve klasse heb je:

- een `enum class State` met de toestanden uit het STD;
- eventbits;
- een constructor die als laatste de taak start met `xTaskCreatePinnedToCore`;
- de STD-interface als publieke functies;
- een `main()` met een `switch (state)`.

De passieve controls (InitControl, BalansControl, BalansverliesControl) volgen
hun activity diagram voor `verwerkMeting()`.

## Bouwen en flashen

```bash
cd esp32s3            # of: cd cyd
idf.py set-target esp32s3   # cyd: idf.py set-target esp32
idf.py build flash monitor
```

`sdkconfig.defaults` zet onder andere de FreeRTOS-tick op 1000 Hz en verdeelt
de taken over de cores. Heb je al een `sdkconfig`, verwijder die dan één keer
zodat de defaults worden overgenomen.

De CYD haalt `espressif/esp_lcd_ili9341` op via de component manager
(`cyd/main/idf_component.yml`), net als in het lesmateriaal.

Welke diagrammen je moet bijwerken zodat ze weer precies met de code
overeenkomen, staat in `DIAGRAM-AANPASSINGEN.md`.

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
| 1 | Mpu6050 | 9 | Meet precies iedere REGELPERIODE (`xTaskDelayUntil`) |
| 0 | VerstuurControl | 5 | ESP-NOW, naast de WiFi-taak van ESP-IDF |

Op core 1 draait alleen de regellus. Core 0 doet alle communicatie: daar
draaien ook de WiFi-taak, `esp_timer` en de FreeRTOS-timertaak van ESP-IDF.
WiFi wordt gestart in de constructor van EspNowZender (in `app_main`, core 0).
Zo kan een storing in ESP-NOW de regeling niet vertragen (NFR-02).

### CYD

| Core | Taak | Prioriteit |
| :--- | :--- | :--- |
| 0 | TonenControl | 5 |
| 0 | ScoreControl | 4 |
| 0 | Touchscreen | 3 |
| 1 | Display | 5 |

Display heeft core 1 voor zichzelf: één `present()` van het hele scherm kost
ruim 40 ms.

## Pinnen (gemeten met de testcode)

ESP32-S3 (`esp32s3/main/main.cpp`):

| Signaal | GPIO | Opmerking |
| :--- | :--- | :--- |
| MPU6050 SDA / SCL | 8 / 9 | adres 0x68 |
| Linkermotor BI / FI | 5 / 4 | BI hoog = vooruit |
| Rechtermotor BI / FI | 7 / 6 | BI hoog = vooruit |

Vooruit is richting de kant met het scherm.

CYD: de pinnen van het scherm komen uit het lesmateriaal. Het touchscreen
gebruikt alleen T_IRQ (GPIO36) van de XPT2046: laag = aangeraakt. Heb je het
2,4 inch-scherm, haal dan `#define DISPLAY28` weg in `Ili9341Display.hpp`.

## ESP-NOW

De S3 stuurt rechtstreeks naar het MAC-adres van de CYD (`CYD_MAC` in
`components/protocol/Bericht.hpp`, in beide projecten gelijk houden). De CYD
bevestigt dan elk bericht, en de S3 logt `CYD bereikbaar` of `CYD niet
bereikbaar` zodra dat verandert. Gebruik je een andere CYD, zet dan zijn MAC-adres in `CYD_MAC`
(staat bij het opstarten in de monitor van de CYD), of zet alles op 0 voor
broadcast.

## Meetgegevens die in de code zitten

| Meting | Uitkomst | Waar in de code |
| :--- | :--- | :--- |
| Kantelas | y-as; voorover positief (+80 graden plat voorover, -53 graden achterover) | `BalansFilter` |
| Meetperiode | 5 ms (200 Hz) haalbaar | `REGELPERIODE` |
| Gyro-ruis stil in de hand | tot ca. 4,5 graden/s | `STILGRENS` = 10 |
| Motorrichting | BI = vooruit, FI = achteruit | pinnen in `main.cpp` |
| Startwaarde motoren | PWM 160 van 255 bij 10 kHz (wielen vrij) | `MOTOR_STARTWAARDE` = 627 van 1000, `PWM_FREQUENTIE_HZ` |

## Hardware tegen storing en wegschieten

- 10 kOhm pull-down van elke ZK5AD-ingang (GPIO 4, 5, 6, 7) naar GND. Tijdens
  een reset of het opstarten van de S3 zweven die pinnen ca. 0,1 s; zonder
  pull-down kunnen de motoren dan vol gaan draaien.
- 100 nF keramisch direct over de aansluitingen van elke motor, en 470 uF of
  meer over de voedingsingang van de ZK5AD.
- GND van ZK5AD en S3 op één punt verbinden; I2C-draden kort en weg van de
  motordraden.

## PID afstellen

Exacte PID-waarden kun je niet uitrekenen zonder model van de robot (massa,
zwaartepunt, wielen, motoren). `PID_KP` = 80, `PID_KI` = 0 en `PID_KD` = 5 zijn
startwaarden met het goede teken, geschat uit het 3D-model (zie de uitleg in Ontwerpparameters.hpp). Alle waarden staan in
`components/core/Ontwerpparameters.hpp`.

**Bij het opstarten:** houd de robot 2 seconden stil rechtop, in de stand
waarin hij in balans is. Dat is het nulpunt (UC-S3-01). Pas daarna gaat hij
balanceren.

1. **Richtingstest (wielen vrij).** Houd de robot vast, laat hem kalibreren en
   kantel hem een paar graden voorover. Beide wielen moeten **vooruit**
   draaien (richting scherm). Achterover moeten ze achteruit draaien. Gaat het
   andersom, laat het weten; verander dan niet zomaar de tekens.
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
(`drawPixel`, `fillScreen`, `present`, de drawables) is gelijk gebleven.

De constructor van Display roept `lcd.init()` aan, en Display is het eerste
object in `app_main`. Zo worden beide helften gereserveerd voordat WiFi en de
andere taken geheugen nemen.

## Monitor op Windows

Blokkeert Windows `xtensa-esp32-elf-addr2line` ("geblokkeerd door een beleid
voor toepassingsbeheer"), start de monitor dan zonder adresvertaling:

```powershell
idf.py -p COM6 monitor -d
```
