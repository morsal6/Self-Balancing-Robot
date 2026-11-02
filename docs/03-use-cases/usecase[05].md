| **[UC05] Gezichtsuitdrukking tonen** | |
| :--- | :--- |
| **Beschrijving** | De CYD toont een animatie of gezichtsuitdrukking die past bij de balanswaarde van de robot. |
| **Pre-condities** | De CYD is ingeschakeld en heeft een geldige balanswaarde ontvangen. |
| **Scenario** | 1. De CYD leest de ontvangen balanswaarde uit.<br>2. De CYD bepaalt welke expressie bij de waarde hoort.<br>3. De CYD toont de bijbehorende animatie of gezichtsuitdrukking.<br>4. De CYD werkt de expressie bij wanneer een nieuwe balanswaarde binnenkomt. |
| **Post-condities** | De CYD toont een expressie die past bij de laatst ontvangen geldige balanswaarde. |
| **Uitzonderingen** | • **Geen recente balanswaarde ontvangen:** De CYD toont een algemene expressie.<br>|
