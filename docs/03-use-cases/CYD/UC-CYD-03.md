| **[UC-CYD-03] Communicatieverlies tonen** | |
| :--- | :--- |
| **Actor** | ESP-NOW-verbinding, gebruiker |
| **Beschrijving** | Wanneer de CYD ESP32 geen geldig bericht meer ontvangt, maakt het scherm zichtbaar dat de getoonde informatie niet actueel is. |
| **Pre-condities** | De CYD ESP32 is geïnitialiseerd en wacht op of heeft eerder een geldig bericht ontvangen. |
| **Scenario** | 1. Er verstrijkt de ingestelde timeout zonder geldig bericht.<br>2. De laatst bekende balansinformatie wordt als niet-actueel gemarkeerd.<br>3. Een communicatieverlies- of wachtexpressie wordt getoond.<br>4. De laatst bekende score blijft onderaan zichtbaar, maar wordt niet verhoogd zolang er geen geldig bericht is.<br>5. Bij een nieuw geldig bericht wordt UC-CYD-01 hervat. |
| **Invariant** | Oude balansdata wordt nooit zonder waarschuwing als actuele data getoond. |
| **Post-condities** | De gebruiker kan onderscheid maken tussen actuele robotdata en een verbroken verbinding. |
| **Uitzonderingen** | • **De verbinding wordt hersteld:** de nieuwe geldige gegevens worden gebruikt voor de gezichtsexpressie en score; balansverliezen tijdens de onderbreking worden via de valteller alsnog geteld.<br>• **De verbinding blijft verbroken:** de communicatieverlies-expressie blijft zichtbaar. |