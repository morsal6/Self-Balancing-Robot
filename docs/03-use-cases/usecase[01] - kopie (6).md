
| **[UC01] Robotonderdeel uitschakelen** | |
| :--- | :--- |
| **Beschrijving** | De gebruiker schakelt de robotonderdelen afzonderlijk uit met hun eigen schakelaar. |
| **Pre-condities** | Het betreffende onderdeel is ingeschakeld. |
| **Scenario** | 1. De gebruiker zet de schakelaar van het onderdeel uit.<br>2. De voeding van dat onderdeel wordt onderbroken.<br>3. Als de motorvoeding wordt uitgeschakeld, stoppen de motoren met aandrijven.<br>4. Als de ESP32-S3 wordt uitgeschakeld, stopt de balansregeling.<br>5. Als de CYD wordt uitgeschakeld, stopt de gezichtsanimatie. |
| **Post-condities** | Het onderdeel waarvan de schakelaar is uitgezet, is uitgeschakeld. Andere onderdelen kunnen ingeschakeld blijven. |
| **Uitzonderingen** | • **ESP32-S3 uitgeschakeld:** De robot balanceert niet meer.<br>• **Motorvoeding uitgeschakeld:** De motoren kunnen de robot niet meer balanceren.<br>• **CYD uitgeschakeld:** De balansregeling blijft werken zolang de ESP32-S3 en motorvoeding ingeschakeld zijn. |

| **[UC02] Robotonderdeel inschakelen** | |
| :--- | :--- |
| **Beschrijving** | De gebruiker schakelt de robotonderdelen afzonderlijk in met hun eigen schakelaar. |
| **Pre-condities** | De benodigde batterijen zijn aangesloten en de schakelaars van de betreffende onderdelen staan uit. |
| **Scenario** | 1. De gebruiker schakelt de ESP32-S3 en de motorvoeding in.<br>2. De ESP32-S3 start op en initialiseert de MPU6050 en motorsturing.<br>3. De ESP32-S3 controleert of de MPU6050 geldige meetwaarden levert.<br>4. Als de sensor gereed is, start de balansregeling automatisch.<br>5. De gebruiker kan de CYD afzonderlijk inschakelen.<br>6. De CYD start op en probeert verbinding te maken met de ESP32-S3 via ESP-NOW. |
| **Post-condities** | De ESP32-S3 balanceert automatisch als de sensor en motorsturing gereed zijn. De CYD kan afzonderlijk verbinding maken en balansinformatie ontvangen. |
| **Uitzonderingen** | • **MPU6050 niet beschikbaar:** De motoren blijven uitgeschakeld en de balansregeling start niet.<br>• **CYD uitgeschakeld of niet verbonden:** De ESP32-S3 blijft zelfstandig balanceren.<br>• **Lage acculading:** Het systeem meet de acculading niet en kan daarom niet op basis van een accupercentage starten weigeren. |

| **[UC03] Automatisch balanceren** | |
| :--- | :--- |
| **Beschrijving** | De ESP32-S3 gebruikt de MPU6050 als  PID-regelaar om de robot met de motoren in balans te houden. |
| **Pre-condities** | De ESP32-S3 en motorvoeding zijn ingeschakeld. De MPU6050 is geïnitialiseerd en levert geldige meetwaarden. |
| **Scenario** | 1. De ESP32-S3 leest de MPU6050 uit.<br>2. De PID-regelaar vergelijkt de gemeten hoek met de gewenste balanshoek.<br>3. De PID-regelaar berekent de benodigde motorsturing.<br>4. De ESP32-S3 stuurt de motoren aan via de H-brug.<br>5. De ESP32-S3 herhaalt deze stappen zolang de balansregeling actief is. |
| **Post-condities** | De motoren worden aangestuurd om de robot rechtop te houden. |
| **Uitzonderingen** | • **Ongeldige of ontbrekende sensordata:** De ESP32-S3 stopt de motorsturing.<br>

| **[UC04] Balansgegevens naar de CYD sturen** | |
| :--- | :--- |
| **Beschrijving** | De ESP32-S3 verstuurt de balanswaarde van de robot naar de CYD via ESP-NOW. |
| **Pre-condities** | De ESP32-S3 is ingeschakeld en heeft een geldige balanswaarde beschikbaar. |
| **Scenario** | 1. De ESP32-S3 bepaalt de actuele balanswaarde.<br>2. De ESP32-S3 verstuurt de waarde via ESP-NOW.<br>3. De CYD ontvangt het bericht.<br>4. De CYD controleert of het bericht geldig is en gebruikt de waarde voor de gezichtsuitdrukking. |
| **Post-condities** | De CYD heeft een recente balanswaarde ontvangen of de verbinding als niet beschikbaar herkend. |
| **Uitzonderingen** | • **CYD uitgeschakeld of niet bereikbaar:** De ESP32-S3 blijft zelfstandig balanceren.<br>• **Ongeldig of ontbrekend bericht:** De CYD gebruikt de waarde niet als actuele balanswaarde en displayed een algemene expressie. |

| **[UC05] Gezichtsuitdrukking tonen** | |
| :--- | :--- |
| **Beschrijving** | De CYD toont een animatie of gezichtsuitdrukking die past bij de balanswaarde van de robot. |
| **Pre-condities** | De CYD is ingeschakeld en heeft een geldige balanswaarde ontvangen. |
| **Scenario** | 1. De CYD leest de ontvangen balanswaarde uit.<br>2. De CYD bepaalt welke expressie bij de waarde hoort.<br>3. De CYD toont de bijbehorende animatie of gezichtsuitdrukking.<br>4. De CYD werkt de expressie bij wanneer een nieuwe balanswaarde binnenkomt. |
| **Post-condities** | De CYD toont een expressie die past bij de laatst ontvangen geldige balanswaarde. |
| **Uitzonderingen** | • **Geen recente balanswaarde ontvangen:** De CYD toont een algemene expressie.<br>|

| **[UC06] Omvallen detecteren en veilig stoppen** | |
| :--- | :--- |
| **Beschrijving** | De ESP32-S3 stopt de motorsturing wanneer de robot omvalt of buiten de ingestelde veilige hellingshoek komt. |
| **Pre-condities** | De ESP32-S3 is ingeschakeld en ontvangt meetwaarden van de MPU6050. |
| **Scenario** | 1. De ESP32-S3 bepaalt de actuele hellingshoek.<br>2. De ESP32-S3 vergelijkt de hellingshoek met de ingestelde veilige grens.<br>3. Als de grens wordt overschreden, schakelt de ESP32-S3 de motorsturing uit.<br>4. De ESP32-S3 stuurt zo mogelijk een foutstatus naar de CYD.<br>5. De gebruiker zet de robot weer rechtop voordat de balansregeling opnieuw wordt gestart. |
| **Post-condities** | De motoren worden niet aangestuurd zolang de robot buiten de ingestelde veilige hellingshoek staat. |
| **Uitzonderingen** | • **Juiste hellingshoek:** Wanneer de juiste hellingshoek wordt aangehouden zal het systeem niet in deze toestand verkeren|
