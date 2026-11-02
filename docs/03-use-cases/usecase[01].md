| **[UC01] Robotonderdeel uitschakelen** | |
| :--- | :--- |
| **Doel** | Een afzonderlijk robotonderdeel uitschakelen met de eigen schakelaar. |
| **Beschrijving** | De gebruiker schakelt een robotonderdeel uit met de eigen schakelaar van dat onderdeel. |
| **Pre-condities** | Het betreffende onderdeel is ingeschakeld. |
| **Scenario** | 1. De gebruiker zet de schakelaar van het onderdeel uit.<br>2. De voeding van dat onderdeel wordt onderbroken.<br>3. Als de gebruiker de motorvoeding uitschakelt, stoppen de motoren met aandrijven.<br>4. Als de gebruiker de ESP32-S3 uitschakelt, stopt de balansregeling.<br>5. Als de gebruiker de CYD uitschakelt, stopt de gezichtsanimatie. |
| **Post-condities** | Het onderdeel waarvan de schakelaar is uitgezet, is uitgeschakeld. Andere onderdelen blijven ingeschakeld zolang hun eigen schakelaar aanstaat. |
| **Uitzonderingen** | • **ESP32-S3 uitgeschakeld:** De robot balanceert niet meer.<br>• **Motorvoeding uitgeschakeld:** De motoren kunnen de robot niet meer balanceren.<br>• **CYD uitgeschakeld:** De balansregeling blijft werken zolang de ESP32-S3 en motorvoeding ingeschakeld zijn. |