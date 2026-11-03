# Use cases

## Introductie

## Use Case Beschrijvingen



## ESP32-CYD




## ESP32-S3

| **[UC01] uitschakelen doormiddel van switch** | |
| :--- | :--- |
| **Beschrijving** |De voeding wordt van de micro |
| **Pre-condities** | Microcontroller moet actief zijn en batterijpercentage boven 10 procent . |
| **Scenario** | 1. Wacht tot de MPU6050 een andere (afwijkende) waarde detecteert.<br>2. Vergelijk waarde met bekende waarde uit database.<br>3. Activeer een bijpassende led indicator.<br>4. Start met het afspelen van het bijpassende audiobestand via de speaker.<br>5. Wacht tot het audiobestand volledig is afgerond. <br>6. Schakel de LED uit en keer terug naar de initiële stand-by modus om te wachten op een nieuwe beweging. |
| **Post-condities** | Het audiobestand van de bijbehorende waarde combinatie is succesvol en volledig afgespeeld. Microcontroller gaat naar stand by modus. |
| **Uitzonderingen** | • **Ondersteboven gehouden:** Indien de knuffel tijdens stap 4 of 5 ondersteboven wordt gehouden, pauzeert het systeem onmiddellijk het afspelen van de audio.<br>• **Lage batterij:** Als bij stap 1 blijkt dat de batterijpercentage te laag is, weigert het systeem het verhaal te starten, knippert de LED drie keer rood om een foutmelding aan te geven en wordt het proces gecancelled. |

UC01_start





