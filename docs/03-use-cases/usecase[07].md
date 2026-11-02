| **[UC07] Balansstatistieken tonen** | |
| :--- | :--- |
| **Actor** | Gebruiker (kijkt naar de CYD, indirect via UC05) |
| **Doel** | Het scoreboard zichtbaar maken naast het gezicht. |
| **Beschrijving** | De huidige balanstijd, het record en het aantal keer omgevallen staan op een vaste plek op het scherm, onder het gezicht. |
| **Pre-condities** | Gezichtsuitdrukking tonen (UC05) is actief. |
| **Scenario** | 1. De statistieken uit de ontvangen balansgegevens worden gelezen.<br>2. De huidige balanstijd, het record en het aantal keer omgevallen worden op de vaste plek getoond.<br>3. Als het record verbroken is, wordt dat kort gemarkeerd (bijvoorbeeld "NIEUW RECORD").<br>4. Bij nieuwe balansgegevens wordt verdergegaan bij stap 1. |
| **Invariant** | Het tonen van de statistieken verstoort de animatie van het gezicht niet. |
| **Post-condities** | De getoonde statistieken zijn die van de laatst ontvangen geldige balansgegevens. |
| **Uitzonderingen** | • **Geen recente balansgegevens (verouderd):** de laatst bekende statistieken blijven zichtbaar en worden gemarkeerd als verouderd (bijvoorbeeld grijs). |