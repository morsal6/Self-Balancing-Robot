| **[UC02] Balanceren** | |
| :--- | :--- |
| **Actor** | Gebruiker |
| **Doel** | De robot rechtop houden. |
| **Beschrijving** | De hellingshoek wordt voortdurend gemeten en de wielen draaien zo dat de robot onder zijn zwaartepunt blijft. |
| **Pre-condities** | Opstarten (UC01) is voltooid: de sensor levert geldige metingen en de robot staat rechtop. |
| **Scenario** | 1. De huidige balanstijd begint te lopen (zie UC06).<br>2. De hellingshoek wordt gemeten.<br>3. De afwijking ten opzichte van de gewenste hoek (rechtop) wordt bepaald.<br>4. Op basis van de grootte van de afwijking, de snelheid waarmee de afwijking verandert en de tijd dat de afwijking aanhoudt, wordt bepaald hoe snel en in welke richting de wielen draaien.<br>5. De wielen draaien volgens die bepaling, zodat de afwijking kleiner wordt.<br>6. Ga verder bij stap 2. |
| **Extension points** | Hellingshoek buiten de veilige grens: Omvallen afhandelen (UC04) |
| **Invariant** | Stappen 2 tot en met 6 worden continu herhaald met vaste, korte tussenpozen. Het balanceren wordt nooit vertraagd door andere activiteiten, zoals het versturen van gegevens of het bijhouden van statistieken. |
| **Post-condities** | Het balanceren stopt alleen bij omvallen (UC04) of bij een sensorfout. De wielen staan dan stil. |
| **Uitzonderingen** | • **Sensor levert ongeldige of geen metingen:** de wielen stoppen en de status FOUT wordt verstuurd. Zodra de sensor weer geldige metingen levert en de robot rechtop wordt gehouden, start het balanceren automatisch opnieuw (UC01, stap 5 en 6). De huidige balanstijd stopt, maar het aantal keer omgevallen neemt niet toe. |