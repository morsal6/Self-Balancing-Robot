# Diagrammen bijwerken naar de code

Dit is alles waarin de code afwijkt van de v2-diagrammen. Na deze aanpassingen
komen de diagrammen en de code weer precies overeen.

## STD's (nodig)

| Diagram | Aanpassing | Waarom |
| :--- | :--- | :--- |
| STD TonenControl | Bij de `[else]`-pijl van d1 ook `laatsteSessieId = bericht.sessieId; laatsteVolgnummer = bericht.volgnummer;` | Anders vergelijkt `isGeldig()` altijd met 0 en worden dubbele of oude berichten nooit herkend |
| STD ScoreControl | In d1 een extra tak `[info.valteller == refValteller]` terug naar Wachten, zonder actie | Anders tekent het Display de score 20 keer per seconde opnieuw |
| STD Display | Pijl `Uit -> Gereed`: `lcd.init();` weghalen. Over blijft `lcd.backlightOn(); lcd.fillScreen(ACHTERGROND); lcd.present();` | `lcd.init()` staat in de constructor van Display, omdat de backbuffer anders niet meer in het geheugen past (naast WiFi) |

## STD's (verduidelijking, mag)

Deze punten veranderen niets aan de toestanden of pijlen. Ze maken alleen
expliciet wat de code doet waar het STD niets over zegt. Een notitie bij het
STD is genoeg.

| Diagram | Notitie | Waarom |
| :--- | :--- | :--- |
| STD Mpu6050 | `after(REGELPERIODE)` telt vanaf het vorige meetmoment (`xTaskDelayUntil`), niet vanaf het einde van de vorige meting | Zo blijft de periode 5 ms, ook als het uitlezen wat tijd kost |
| STD TonenControl | Komen een bericht en de commTimer tegelijk binnen, dan gaat het bericht voor | Een geldig bericht herstart de timer toch |
| STD ScoreControl | Komen een aanraking en een nieuwe valteller tegelijk binnen, dan gaat de aanraking voor | De nieuwe valteller wordt daarna alsnog verwerkt |
| STD Display | Komen een score en een gezicht tegelijk binnen, dan gaat de score voor | Het gezicht wordt daarna alsnog getekend |
| STD VerstuurControl | Komen `after(VERZENDPERIODE)` en `verstuur()` tegelijk, dan wordt er één bericht verstuurd | Beide pijlen gaan via merge node m1 naar dezelfde actie |

## Klassediagram ESP32-S3

| Klasse | Aanpassing | Waarom |
| :--- | :--- | :--- |
| RobotControl | Gestippelde pijl (dependency) naar Mpu6050 | RobotControl meldt zich in zijn constructor aan als listener (`mpu6050.addListener(this)`), stap 5c van de reader |
| Mpu6050 | Attribuut `- apparaat: i2c_master_dev_handle_t` | Het I2C-apparaat, nodig in `leesSensor()` |
| MotorDriver | Attributen `- vorigeLinks: int32_t` en `- vorigeRechts: int32_t` | Begrenzing van stroompieken bij het omkeren van de motoren |

## Klassediagram CYD

| Klasse | Aanpassing | Waarom |
| :--- | :--- | :--- |
| Touchscreen | Attribuut `- irqPin: gpio_num_t` | De T_IRQ-pin van de XPT2046 |
| Ili9341Display | `- backbuffer_: uint16_t*` wordt `- backbuffer_: uint16_t*[2]` (boven- en onderhelft) | Eén blok van 153.600 bytes past niet naast WiFi; twee blokken van 76.800 bytes wel |
| Drawables | `Square` komt niet voor | Niet gebruikt; ook uit de code gehaald |

## Niet in de diagrammen (hoort er ook niet in)

- RTOS-handles: `taskHandle`, `eventGroup`, queues, timers en de mutexen van RobotFase, Sessie en BalansData.
- De variabele `state`, `taskWrapper()`, `main()` en timer-callbacks.
- Hulpfuncties in een naamloze namespace in een `.cpp` (bijvoorbeeld `sign()`, `zetMotor()`, `tekenBlok()`).
- Ontwerpparameters (`Ontwerpparameters.hpp`). Die horen in het ontwikkeldocument, hoofdstuk Ontwerpparameters.
- Het meetprogramma `test_s3_meting`. Dat is een testhulpmiddel, geen onderdeel van het ontwerp.
