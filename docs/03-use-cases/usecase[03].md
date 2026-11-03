| **[UC03] Balansgegevens versturen** | |
| :--- | :--- |
| **Actor** | Gebruiker (indirect, via UC01) |
| **Doel** | De CYD voorzien van actuele balansgegevens voor het gezicht en de statistieken. |
| **Beschrijving** | De balansgegevens worden regelmatig draadloos verstuurd, vanaf het opstarten van de robot. |
| **Pre-condities** | Opstarten (UC01) heeft het versturen gestart (UC01, stap 2). |
| **Scenario** | 1. De balansgegevens worden samengesteld uit de actuele hellingshoek, de mate van schommelen, de status en de statistieken.<br>2. De balansgegevens worden draadloos verstuurd.<br>3. Na een korte, vaste tijd wordt verdergegaan bij stap 1. |
| **Invariant** | Het versturen heeft altijd lagere voorrang dan het balanceren en vertraagt het balanceren nooit. Tijdens opstarten of een sensorfout bevat het bericht alleen de status (OPSTARTEN of FOUT) en de statistieken. |
| **Post-condities** | Er is een bericht verstuurd, ongeacht of de CYD het heeft ontvangen. |
| **Uitzonderingen** | • **CYD uitgeschakeld of buiten bereik:** versturen mislukt, maar het balanceren gaat ongestoord door. Er blijven berichten worden verstuurd, zodat de CYD direct meedoet zodra hij weer aanstaat. |