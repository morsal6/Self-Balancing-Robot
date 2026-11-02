| **[UC02] Robotonderdeel inschakelen** | |
| :--- | :--- |
| **Beschrijving** | De gebruiker schakelt de robotonderdelen afzonderlijk in met hun eigen schakelaar. |
| **Pre-condities** | De benodigde batterijen zijn aangesloten en de schakelaars van de betreffende onderdelen staan uit. |
| **Scenario** | 1. De gebruiker schakelt de ESP32-S3 en de motorvoeding in.<br>2. De ESP32-S3 start op en initialiseert de MPU6050 en motorsturing.<br>3. De ESP32-S3 controleert of de MPU6050 geldige meetwaarden levert.<br>4. Als de sensor gereed is, start de balansregeling automatisch.<br>5. De gebruiker kan de CYD afzonderlijk inschakelen.<br>6. De CYD start op en probeert verbinding te maken met de ESP32-S3 via ESP-NOW. |
| **Post-condities** | De ESP32-S3 balanceert automatisch als de sensor en motorsturing gereed zijn. De CYD kan afzonderlijk verbinding maken en balansinformatie ontvangen. |
| **Uitzonderingen** | • **MPU6050 niet beschikbaar:** De motoren blijven uitgeschakeld en de balansregeling start niet.<br>• **CYD uitgeschakeld of niet verbonden:** De ESP32-S3 blijft zelfstandig balanceren.<br>• **Lage acculading:** Het systeem meet de acculading niet en kan daarom niet op basis van een accupercentage starten weigeren. |
