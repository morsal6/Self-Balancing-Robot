
| **[UC01] Robotonderdeel uitschakelen** | |
| :--- | :--- |
| **Beschrijving** | De gebruiker schakelt de robotonderdelen afzonderlijk uit met hun eigen schakelaar. |
| **Pre-condities** | Het betreffende onderdeel is ingeschakeld. |
| **Scenario** | 1. De gebruiker zet de schakelaar van het onderdeel uit.<br>2. De voeding van dat onderdeel wordt onderbroken.<br>3. Als de motorvoeding wordt uitgeschakeld, stoppen de motoren met aandrijven.<br>4. Als de ESP32-S3 wordt uitgeschakeld, stopt de balansregeling.<br>5. Als de CYD wordt uitgeschakeld, stopt de gezichtsanimatie. |
| **Post-condities** | Het onderdeel waarvan de schakelaar is uitgezet, is uitgeschakeld. Andere onderdelen kunnen ingeschakeld blijven. |
| **Uitzonderingen** | • **ESP32-S3 uitgeschakeld:** De robot balanceert niet meer.<br>• **Motorvoeding uitgeschakeld:** De motoren kunnen de robot niet meer balanceren.<br>• **CYD uitgeschakeld:** De balansregeling blijft werken zolang de ESP32-S3 en motorvoeding ingeschakeld zijn. |
