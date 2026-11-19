# Diagrammen en code

De STD's en klassediagrammen komen overeen met de code:

- `std_esp32s3_docentstijl_v2.drawio` en `std_cyd_docentstijl_v2.drawio`
- `klassediagrammen_robot_v5.drawio`

Hieronder staat wat er ten opzichte van de vorige versies is veranderd, en
wat bewust niet in de diagrammen staat.

## Nieuw in deze versie (v5)

In het klassediagram van de ESP32-S3 verandert alleen de klasse
PidRegelaar. Pas in je eigen diagram (`klassediagram S3 v3.png`) het vak
PidRegelaar zo aan:

```text
- kp: float
- ki: float
- kd: float
- integraal: float
- vorigeAfwijking: float
- snelheid: float              <- nieuw
- snelheidIntegraal: float     <- nieuw
---------------------------------------------
+ init()
+ bereken(balanswaarde: float): int16_t      <- was bereken(afwijking: float)
+ resetIntegraal()
```

In het klassediagram van de CYD krijgt Ili9341Display twee private
methodes erbij, omdat jouw eigen `drawLine` uit de opdracht "ESP32 CYD
Drawables" ze gebruikt:

```text
- drawLineLow(x0: int, y0: int, x1: int, y1: int, color: uint16_t)
- drawLineHigh(x0: int, y0: int, x1: int, y1: int, color: uint16_t)
```

De STD's en de activity diagrams blijven hetzelfde. BalansControl roept al
`pidRegelaar.bereken(balanswaarde)` aan.

## STD's

| Diagram | Wat er veranderd is | Waarom |
| :--- | :--- | :--- |
| STD TonenControl | Bij `[else]` van de beslisknoop ook `laatsteSessieId = bericht.sessieId; laatsteVolgnummer = bericht.volgnummer;` | Anders vergelijkt `isGeldig()` altijd met 0 en worden dubbele of oude berichten nooit herkend |
| STD TonenControl | Pijl uit de initial node begint met `commTimer = xTimerCreate(...)` | Zelfde volgorde als de code: de timer wordt in de constructor aangemaakt, de rest in `main()` |
| STD ScoreControl | Eén beslisknoop per voorwaarde (d1, d2, d3), met de nieuwe tak `[info.valteller == refValteller]` zonder actie | De guards sluiten elkaar nu uit, net als de `if / else if` in de code. Zonder die tak tekende het Display de score 20 keer per seconde opnieuw |
| STD Display | Pijl `Uit -> Gereed` zonder `lcd.init();` | `lcd.init()` staat in de constructor van Display, omdat de backbuffer anders niet meer in het geheugen past (naast WiFi) |
| InitControl::verwerkMeting | Eén beslisknoop per voorwaarde (d1, d2, d3) | De guards `[!meting.geldig]` en `[abs(meting.gy) > STILGRENS]` sloten elkaar niet uit; nu volgt het diagram de volgorde van de code |
| STD Mpu6050, VerstuurControl, TonenControl, ScoreControl, Display | Notitie onder het STD | Legt uit wat het STD zelf niet laat zien: de timing van `after(REGELPERIODE)` en wat er gebeurt als twee events tegelijk komen |

RobotControl, BalansControl, BalansverliesControl en Touchscreen zijn niet
veranderd.

## Klassediagram ESP32-S3

| Klasse | Wat er veranderd is | Waarom |
| :--- | :--- | :--- |
| RobotControl | Gestippelde pijl (dependency) naar Mpu6050 | RobotControl meldt zich in zijn constructor aan als listener (`mpu6050.addListener(this)`), stap 5c van de reader |
| Mpu6050 | Attribuut `- apparaat: i2c_master_dev_handle_t` | Het I2C-apparaat, nodig in `leesSensor()` |
| MotorDriver | Attributen `- vorigeLinks: int32_t` en `- vorigeRechts: int32_t` | Begrenzing van stroompieken bij het omkeren van de motoren (`MOTOR_MAX_STAP`) |
| PidRegelaar (v5) | Attributen `- snelheid: float` en `- snelheidIntegraal: float` | Snelheidsterugkoppeling: zonder encoders rijdt de robot bij de kleinste fout in het nulpunt steeds harder één kant op en valt (zie README, PID afstellen) |
| PidRegelaar (v5) | `bereken(afwijking: float)` is `bereken(balanswaarde: float)` | De afwijking is nu de balanswaarde min de doelhoek en wordt in `bereken()` uitgerekend; de parameter is de balanswaarde, net als in het activity diagram van BalansControl |

De dode-zonecompensatie (`MOTOR_STARTWAARDE`) zit nu in `PidRegelaar` en niet
meer in `MotorDriver`, omdat de snelheid uit het echte motorcommando wordt
geschat. Aan de klassen in het diagram verandert daardoor niets.

## Klassediagram CYD

| Klasse | Wat er veranderd is | Waarom |
| :--- | :--- | :--- |
| Touchscreen | Attribuut `- irqPin: gpio_num_t` | De T_IRQ-pin van de XPT2046 |
| Ili9341Display | `- backbuffer_: uint16_t*` is `- backbuffer_: uint16_t*[2]` (boven- en onderhelft) | Eén blok van 153.600 bytes past niet naast WiFi; twee blokken van 76.800 bytes wel |
| Display | Geen pijl meer naar Triangle | Display tekent alleen met Circle, Line en Rectangle |
| Ili9341Display (v5) | `- drawLineLow(...)` en `- drawLineHigh(...)` | Hulpmethodes van je eigen `drawLine` (Bresenham voor vlakke en steile lijnen) |

Display gebruikt Circle, Rectangle en Line direct (`Circle(...).draw(lcd)`),
zonder eigen hulpfuncties. In `Ili9341Display` zijn de TODO's uit de
lesdriver ingevuld: `drawLine` (met `drawLineLow` en `drawLineHigh`) en
`drawCircle` komen uit je eigen uitwerking van de opdracht "ESP32 CYD
Drawables"; het vullen van `drawTriangle` is erbij gemaakt.

`Square` staat niet in het diagram en ook niet in de code: hij werd niet
gebruikt.

## Niet in de diagrammen (hoort er ook niet in)

- RTOS-handles: `taskHandle`, `eventGroup`, queues, timers en de mutexen van RobotFase, Sessie en BalansData.
- De variabele `state`, `taskWrapper()`, `main()` en timer-callbacks.
- Hulpfuncties in een naamloze namespace in een `.cpp` (bijvoorbeeld `sign()`, `zetMotor()`, `configureerKanaal()`).
- Constanten in een klasse (`WIDTH`, `HEIGHT`, `HALF_HEIGHT`, `TAG`) en de enums `Fase`, `Resultaat` en `ExpressieSoort`.
- Ontwerpparameters (`Ontwerpparameters.hpp`). Die horen in het ontwikkeldocument, hoofdstuk Ontwerpparameters.
- Het meetprogramma `test_s3_meting`. Dat is een testhulpmiddel, geen onderdeel van het ontwerp.
- De namen van de beslisknopen (d1, d2, ...) staan niet in de STD's; in de code staan ze als commentaar, in volgorde van boven naar beneden.
