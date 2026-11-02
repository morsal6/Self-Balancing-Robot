| **[UC06] Balansstatistieken bijhouden** | |
| :--- | :--- |
| **Actor** | Gebruiker (indirect, via UC02) |
| **Doel** | Een scoreboard bijhouden van hoe goed de robot balanceert. |
| **Beschrijving** | De huidige balanstijd, het record en het aantal keer omgevallen worden bijgehouden tijdens het balanceren. |
| **Pre-condities** | Balanceren (UC02) start of is actief. |
| **Scenario** | 1. Zodra het balanceren begint, loopt de huidige balanstijd vanaf nul.<br>2. Zodra de robot omvalt (UC04), stopt de huidige balanstijd en neemt het aantal keer omgevallen met één toe.<br>3. Als de gestopte balanstijd langer is dan het record, wordt dit het nieuwe record. Het record wordt bewaard nadat de wielen stilstaan.<br>4. Bij het volgende balanceren begint de huidige balanstijd weer bij nul. |
| **Invariant** | Het record blijft behouden na het uitschakelen. De huidige balanstijd en het aantal keer omgevallen beginnen bij elke start van de robot weer op nul. De statistieken zijn onderdeel van elk verstuurd bericht (UC03). |
| **Post-condities** | De statistieken zijn actueel. |
| **Uitzonderingen** | • **Bewaren van het record mislukt:** het record blijft in het werkgeheugen staan tot het uitschakelen. Het balanceren gaat ongestoord door.<br>• **Balanceren stopt door een sensorfout:** de huidige balanstijd stopt, maar het aantal keer omgevallen neemt niet toe en het record wordt niet bijgewerkt. |