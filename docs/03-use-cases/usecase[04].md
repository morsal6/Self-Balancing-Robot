| **[UC04] Balansgegevens naar de CYD sturen** | |
| :--- | :--- |
| **Doel** | De CYD voorzien van actuele balansgegevens voor de gezichtsanimatie. |
| **Beschrijving** | De ESP32-S3 verstuurt de balanswaarde van de robot naar de CYD via ESP-NOW. |
| **Pre-condities** | De ESP32-S3 is ingeschakeld en heeft een geldige balanswaarde beschikbaar. |
| **Scenario** | 1. De ESP32-S3 bepaalt de actuele balanswaarde.<br>2. De ESP32-S3 verstuurt de waarde via ESP-NOW.<br>3. De CYD ontvangt het bericht.<br>4. De CYD controleert of het bericht geldig is en geeft de waarde door aan de gezichtsanimatie. |
| **Post-condities** | De CYD heeft een recente balanswaarde ontvangen of de verbinding als niet beschikbaar herkend. |
| **Uitzonderingen** | • **CYD uitgeschakeld of niet bereikbaar:** De ESP32-S3 blijft zelfstandig balanceren.<br>• **Ongeldig of ontbrekend bericht:** De CYD gebruikt de waarde niet als actuele balanswaarde en toont een algemene expressie. |