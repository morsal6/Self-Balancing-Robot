| **[UC01] Opstarten** | |
| :--- | :--- |
| **Actor** | Gebruiker |
| **Doel** | De robot gereed maken, zodat het balanceren veilig en automatisch kan starten. |
| **Beschrijving** | Na het inschakelen van de voeding wordt de sensor gestart en gekalibreerd. Het balanceren start zodra de gebruiker de robot rechtop houdt. |
| **Pre-condities** | De voeding van de besturing en de motorvoeding zijn ingeschakeld (elk met een eigen schakelaar). |
| **Scenario** | 1. De sensor en de motorsturing worden gestart. De wielen blijven stilstaan.<br>2. Het periodiek versturen van balansgegevens (UC03) start.<br>3. Er wordt gecontroleerd of de sensor geldige metingen levert.<br>4. Zodra de robot stilstaat, wordt het nulpunt van de sensor bepaald (kalibratie).<br>5. Er wordt gewacht tot de gebruiker de robot rechtop houdt: de hellingshoek blijft een korte, vaste tijd binnen de startgrens.<br>6. Het balanceren (UC02) start automatisch. |
| **Invariant** | Zolang het opstarten niet is voltooid, blijven de wielen stilstaan. |
| **Post-condities** | Balanceren (UC02) is actief en de balansgegevens worden periodiek verstuurd (UC03). |
| **Uitzonderingen** | • **Sensor reageert niet of levert ongeldige metingen:** de wielen blijven stilstaan en de status FOUT wordt verstuurd. Na korte tijd wordt de controle van stap 3 opnieuw uitgevoerd, totdat de sensor weer geldige metingen levert.<br>• **Robot beweegt tijdens de kalibratie:** de kalibratie wordt afgebroken en opnieuw uitgevoerd zodra de robot stilstaat. |