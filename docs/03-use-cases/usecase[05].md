| **[UC05] Gezichtsuitdrukking tonen** | |
| :--- | :--- |
| **Actor** | Gebruiker (kijkt naar de CYD) |
| **Doel** | De balansstatus van de robot zichtbaar maken met een passend gezicht. |
| **Beschrijving** | Het gezicht (ogen en expressie) laat zien hoe stabiel de robot balanceert, op basis van de ontvangen balansgegevens. |
| **Pre-condities** | De CYD is ingeschakeld (eigen schakelaar). |
| **Scenario** | 1. Een neutraal gezicht wordt getoond (algemene expressie).<br>2. Het tonen van de balansstatistieken (UC07) start.<br>3. Er wordt gewacht op nieuwe balansgegevens.<br>4. Er wordt gecontroleerd of de balansgegevens geldig zijn (volledig en niet verouderd).<br>5. De expressie wordt bepaald volgens de expressietabel hieronder.<br>6. Het gezicht gaat vloeiend over naar die expressie.<br>7. Ga verder bij stap 3. |
| **Expressietabel** | • **Status OPSTARTEN:** wakker wordende ogen (half dicht, knipperend).<br>• **Status BALANCEREN, rustig (kleine hellingshoek, weinig schommelen):** blije, rustige ogen.<br>• **Status BALANCEREN, matig schommelen:** alerte ogen die kijken in de richting waarheen de robot helt.<br>• **Status BALANCEREN, sterk schommelen of dicht bij de veilige grens:** grote, bange ogen.<br>• **Status OMGEVALLEN:** gesloten ogen of kruisjes als ogen.<br>• **Status FOUT:** verward gezicht.<br>• **Geen geldige of verouderde gegevens:** neutraal gezicht (algemene expressie). |
| **Invariant** | De ogen knipperen met onregelmatige tussenpozen, onafhankelijk van binnenkomende gegevens, zodat het gezicht levend blijft. |
| **Post-condities** | Het gezicht toont de expressie die hoort bij de laatst ontvangen geldige balansgegevens. |
| **Uitzonderingen** | • **Ongeldig of onvolledig bericht:** het bericht wordt genegeerd. De vorige expressie blijft zichtbaar totdat de gegevens verouderd zijn.<br>• **Geen recente balansgegevens (verouderd):** het neutrale gezicht wordt getoond. Zodra weer een geldig bericht binnenkomt, wordt de normale werking hervat. |