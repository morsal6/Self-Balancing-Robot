| **[UC-CYD-03] Communicatieverlies tonen** | |
| :--- | :--- |
| **Actor** | Gebruiker |
| **Beschrijving** | Maakt op het scherm zichtbaar dat de getoonde informatie niet meer actueel is, omdat er geen geldige berichten meer binnenkomen. |
| **Pre-condities** | UC-CYD-01 voert deze use case uit, omdat er binnen de communicatietimeout geen geldig bericht is ontvangen. |
| **Scenario** | 1. De laatst bekende balansinformatie wordt als niet-actueel gemarkeerd.<br>2. In het bovenste deel van het scherm wordt een communicatieverlies-expressie getoond. De score onderaan blijft zichtbaar. |
| **Invariant** | Oude balansdata wordt nooit zonder waarschuwing als actuele data getoond. |
| **Post-condities** | De communicatieverlies-expressie blijft zichtbaar totdat UC-CYD-01 weer een geldig bericht ontvangt. Terug naar UC-CYD-01. |