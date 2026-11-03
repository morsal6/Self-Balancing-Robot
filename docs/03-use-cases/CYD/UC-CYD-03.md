| **[UC-CYD-03] Communicatieverlies tonen** | |
| :--- | :--- |
| **Actor** | ESP-NOW-verbinding, gebruiker |
| **Beschrijving** | Wanneer de CYD ESP32 geen geldig bericht meer ontvangt, maakt het scherm zichtbaar dat de getoonde balansinformatie niet actueel is. |
| **Pre-condities** | De CYD ESP32 is geïnitialiseerd en wacht op of heeft eerder een geldig bericht ontvangen. |
| **Scenario** | 1. Er verstrijkt de ingestelde timeout zonder geldig bericht.<br>2. De laatst bekende balansinformatie wordt als niet-actueel gemarkeerd.<br>3. Een communicatieverlies- of wachtindicatie wordt getoond.<br>4. Het scoreboard wordt niet verhoogd zolang geen geldige nieuwe balansinformatie beschikbaar is.<br>5. Bij een nieuw geldig bericht wordt UC-CYD-01 hervat. |
| **Invariant** | Oude balansdata wordt nooit zonder waarschuwing als actuele data getoond. |
| **Post-condities** | De gebruiker kan onderscheid maken tussen actuele robotdata en een verbroken verbinding. |
| **Uitzonderingen** | • **De verbinding wordt hersteld:** de nieuwe geldige balansgegevens worden gebruikt voor de gezichtsexpressie en score.<br>• **De verbinding blijft verbroken:** de wacht- of communicatieverlies-indicatie blijft zichtbaar. |