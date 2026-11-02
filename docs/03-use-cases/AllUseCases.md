| **[UC01] Robotonderdeel uitschakelen** | |
| :--- | :--- |
| **Doel** | Een afzonderlijk robotonderdeel uitschakelen met de eigen schakelaar. |
| **Beschrijving** | De gebruiker schakelt een robotonderdeel uit met de eigen schakelaar van dat onderdeel. |
| **Pre-condities** | Het betreffende onderdeel is ingeschakeld. |
| **Scenario** | 1. De gebruiker zet de schakelaar van het onderdeel uit.<br>2. De voeding van dat onderdeel wordt onderbroken.<br>3. Als de gebruiker de motorvoeding uitschakelt, stoppen de motoren met aandrijven.<br>4. Als de gebruiker de ESP32-S3 uitschakelt, stopt de balansregeling.<br>5. Als de gebruiker de CYD uitschakelt, stopt de gezichtsanimatie. |
| **Post-condities** | Het onderdeel waarvan de schakelaar is uitgezet, is uitgeschakeld. Andere onderdelen blijven ingeschakeld zolang hun eigen schakelaar aanstaat. |
| **Uitzonderingen** | • **ESP32-S3 uitgeschakeld:** De robot balanceert niet meer.<br>• **Motorvoeding uitgeschakeld:** De motoren kunnen de robot niet meer balanceren.<br>• **CYD uitgeschakeld:** De balansregeling blijft werken zolang de ESP32-S3 en motorvoeding ingeschakeld zijn. |

| **[UC02] Robotonderdeel inschakelen** | |
| :--- | :--- |
| **Doel** | De robotonderdelen inschakelen, zodat de balansregeling automatisch kan starten en de CYD balansinformatie kan tonen. |
| **Beschrijving** | De gebruiker schakelt de robotonderdelen afzonderlijk in met hun eigen schakelaar. |
| **Pre-condities** | De benodigde batterijen zijn aangesloten en de schakelaars van de betreffende onderdelen staan uit. |
| **Scenario** | 1. De gebruiker schakelt de ESP32-S3 en de motorvoeding in.<br>2. De ESP32-S3 start op en initialiseert de MPU6050 en de motorsturing.<br>3. De ESP32-S3 controleert of de MPU6050 geldige meetwaarden levert.<br>4. Als de sensor gereed is, start de balansregeling automatisch.<br>5. De gebruiker kan de CYD afzonderlijk inschakelen.<br>6. De CYD start op en probeert via ESP-NOW verbinding te maken met de ESP32-S3. |
| **Post-condities** | De ESP32-S3 balanceert automatisch als de sensor en motorsturing gereed zijn. De CYD kan afzonderlijk verbinding maken en balansinformatie ontvangen. |
| **Uitzonderingen** | • **MPU6050 niet beschikbaar:** De motoren blijven uitgeschakeld en de balansregeling start niet.<br>• **CYD uitgeschakeld of niet verbonden:** De ESP32-S3 blijft zelfstandig balanceren.<br>• **Lage acculading:** De acculading wordt niet gemeten. Daardoor kan de gebruiker geen accupercentage zien en wordt starten niet op basis van een gemeten acculading geweigerd. |

| **[UC03] Automatisch balanceren** | |
| :--- | :--- |
| **Doel** | De robot met behulp van de MPU6050, de PID-regelaar en de motoren rechtop houden. |
| **Beschrijving** | De ESP32-S3 gebruikt meetwaarden van de MPU6050 en een PID-regelaar om de motoren aan te sturen en de robot in balans te houden. |
| **Pre-condities** | De ESP32-S3 en motorvoeding zijn ingeschakeld. De MPU6050 is geïnitialiseerd en levert geldige meetwaarden. |
| **Scenario** | 1. De ESP32-S3 leest de MPU6050 uit.<br>2. De ESP32-S3 bepaalt de gemeten hellingshoek.<br>3. De PID-regelaar vergelijkt de gemeten hoek met de gewenste balanshoek.<br>4. De PID-regelaar berekent de benodigde motorsturing.<br>5. De ESP32-S3 stuurt de motoren aan via de H-brug.<br>6. De ESP32-S3 herhaalt deze stappen zolang de balansregeling actief is. |
| **Post-condities** | De motoren worden aangestuurd om de robot rechtop te houden. |
| **Uitzonderingen** | • **Ongeldige of ontbrekende sensordata:** De ESP32-S3 stopt de motorsturing. |

| **[UC04] Balansgegevens naar de CYD sturen** | |
| :--- | :--- |
| **Doel** | De CYD voorzien van actuele balansgegevens voor de gezichtsanimatie. |
| **Beschrijving** | De ESP32-S3 verstuurt de balanswaarde van de robot naar de CYD via ESP-NOW. |
| **Pre-condities** | De ESP32-S3 is ingeschakeld en heeft een geldige balanswaarde beschikbaar. |
| **Scenario** | 1. De ESP32-S3 bepaalt de actuele balanswaarde.<br>2. De ESP32-S3 verstuurt de waarde via ESP-NOW.<br>3. De CYD ontvangt het bericht.<br>4. De CYD controleert of het bericht geldig is en geeft de waarde door aan de gezichtsanimatie. |
| **Post-condities** | De CYD heeft een recente balanswaarde ontvangen of de verbinding als niet beschikbaar herkend. |
| **Uitzonderingen** | • **CYD uitgeschakeld of niet bereikbaar:** De ESP32-S3 blijft zelfstandig balanceren.<br>• **Ongeldig of ontbrekend bericht:** De CYD gebruikt de waarde niet als actuele balanswaarde en toont een algemene expressie. |

| **[UC05] Gezichtsuitdrukking tonen** | |
| :--- | :--- |
| **Doel** | De balansstatus van de robot zichtbaar maken met een passende gezichtsuitdrukking op de CYD. |
| **Beschrijving** | De CYD toont een animatie of gezichtsuitdrukking die past bij de ontvangen balanswaarde. |
| **Pre-condities** | De CYD is ingeschakeld en heeft een geldige balanswaarde ontvangen. |
| **Scenario** | 1. De CYD leest de ontvangen balanswaarde uit.<br>2. De CYD bepaalt welke expressie bij de waarde hoort.<br>3. De CYD toont de bijbehorende animatie of gezichtsuitdrukking.<br>4. De CYD werkt de expressie bij wanneer een nieuwe balanswaarde binnenkomt. |
| **Post-condities** | De CYD toont een expressie die past bij de laatst ontvangen geldige balanswaarde. |
| **Uitzonderingen** | • **Geen recente balanswaarde ontvangen:** De CYD toont een algemene expressie. |

| **[UC06] Omvallen detecteren en veilig stoppen** | |
| :--- | :--- |
| **Doel** | Voorkomen dat de motoren blijven aandrijven wanneer de robot omvalt of te ver overhelt. |
| **Beschrijving** | De ESP32-S3 stopt de motorsturing wanneer de robot buiten de ingestelde veilige hellingshoek komt. |
| **Pre-condities** | De ESP32-S3 is ingeschakeld en ontvangt meetwaarden van de MPU6050. |
| **Scenario** | 1. De ESP32-S3 bepaalt de actuele hellingshoek.<br>2. De ESP32-S3 vergelijkt de hellingshoek met de ingestelde veilige grens.<br>3. Als de grens wordt overschreden, schakelt de ESP32-S3 de motorsturing uit.<br>4. De ESP32-S3 stuurt zo mogelijk een foutstatus naar de CYD.<br>5. De gebruiker zet de robot weer rechtop voordat de balansregeling opnieuw kan starten. |
| **Post-condities** | De motoren worden niet aangestuurd zolang de robot buiten de ingestelde veilige hellingshoek staat. |
| **Uitzonderingen** | • **Veilige hellingshoek aangehouden:** De grens wordt niet overschreden en de motorsturing wordt niet vanwege omvallen uitgeschakeld. |
