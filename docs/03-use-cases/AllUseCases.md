# Use Cases – Zelfbalancerende robot
 
Systeemgrens: **Zelfbalancerende robot** (twee subsystemen: de besturing van sensor en motoren, en de CYD voor gezicht en scoreboard).
Actor: **Gebruiker** (menselijke actor, buiten de systeemgrens).
 
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
 
| **[UC04] Omvallen afhandelen** | |
| :--- | :--- |
| **Actor** | Gebruiker (indirect, via UC02) |
| **Doel** | Voorkomen dat de wielen blijven draaien als de robot omvalt, en de robot klaarzetten voor een nieuwe poging. |
| **Beschrijving** | Zodra de hellingshoek buiten de veilige grens komt, stoppen de wielen. Zodra de gebruiker de robot weer rechtop houdt, start het balanceren automatisch opnieuw. |
| **Pre-condities** | Balanceren (UC02) is actief. |
| **Scenario** | 1. Zodra de hellingshoek buiten de veilige grens komt, stoppen de wielen.<br>2. De status wordt OMGEVALLEN.<br>3. Het omvallen wordt verwerkt in de statistieken (zie UC06).<br>4. Er wordt gewacht tot de gebruiker de robot weer rechtop houdt: de hellingshoek blijft een korte, vaste tijd binnen de startgrens.<br>5. Het balanceren (UC02) start automatisch opnieuw. |
| **Invariant** | Zolang de robot omgevallen is, blijven de wielen stilstaan. |
| **Post-condities** | Het balanceren (UC02) is opnieuw actief, met een huidige balanstijd die weer bij nul begint. |
| **Uitzonderingen** | Geen. Omvallen is in deze use case het normale scenario. |
 
| **[UC05] Gezichtsuitdrukking tonen** | |
| :--- | :--- |
| **Actor** | Gebruiker (kijkt naar de CYD) |
| **Doel** | De balansstatus van de robot zichtbaar maken met een passend gezicht. |
| **Beschrijving** | Het gezicht (ogen en expressie) laat zien hoe stabiel de robot balanceert, op basis van de ontvangen balansgegevens. |
| **Pre-condities** | De CYD is ingeschakeld (eigen schakelaar). |
| **Scenario** | 1. Een neutraal gezicht wordt getoond (algemene expressie).<br>2. Er wordt gewacht op nieuwe balansgegevens.<br>3. Er wordt gecontroleerd of de balansgegevens geldig zijn (volledig en niet verouderd).<br>4. De expressie wordt bepaald volgens de expressietabel hieronder.<br>5. Het gezicht gaat vloeiend over naar die expressie.<br>6. Ga verder bij stap 2. |
| **Expressietabel** | • **Status OPSTARTEN:** wakker wordende ogen (half dicht, knipperend).<br>• **Status BALANCEREN, rustig (kleine hellingshoek, weinig schommelen):** blije, rustige ogen.<br>• **Status BALANCEREN, matig schommelen:** alerte ogen die kijken in de richting waarheen de robot helt.<br>• **Status BALANCEREN, sterk schommelen of dicht bij de veilige grens:** grote, bange ogen.<br>• **Status OMGEVALLEN:** gesloten ogen of kruisjes als ogen.<br>• **Status FOUT:** verward gezicht.<br>• **Geen geldige of verouderde gegevens:** neutraal gezicht (algemene expressie). |
| **Extension points** | Geen. Statistieken tonen (UC07) wordt altijd uitgevoerd tijdens deze use case. |
| **Invariant** | De ogen knipperen met onregelmatige tussenpozen, onafhankelijk van binnenkomende gegevens, zodat het gezicht levend blijft. |
| **Post-condities** | Het gezicht toont de expressie die hoort bij de laatst ontvangen geldige balansgegevens. |
| **Uitzonderingen** | • **Ongeldig of onvolledig bericht:** het bericht wordt genegeerd. De vorige expressie blijft zichtbaar totdat de gegevens verouderd zijn.<br>• **Geen recente balansgegevens (verouderd):** het neutrale gezicht wordt getoond. Zodra weer een geldig bericht binnenkomt, wordt de normale werking hervat. |
 
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
 
---
 
## Relaties voor het use case diagram
 
| Van | Relatie | Naar | Toelichting |
| :--- | :--- | :--- | :--- |
| Gebruiker | associatie | UC01 Opstarten | Top van de hiërarchie. |
| Gebruiker | associatie | UC05 Gezichtsuitdrukking tonen | Top van de hiërarchie. |
| UC01 Opstarten | `<<include>>` | UC02 Balanceren | Opstarten leidt altijd tot balanceren. |
| UC01 Opstarten | `<<include>>` | UC03 Balansgegevens versturen | Het versturen start altijd bij het opstarten en loopt parallel aan het balanceren. Een afwezige CYD is een uitzondering op het versturen, geen reden om het niet uit te voeren. |
| UC02 Balanceren | `<<include>>` | UC06 Balansstatistieken bijhouden | Statistieken worden altijd bijgehouden. |
| UC04 Omvallen afhandelen | `<<extend>>` | UC02 Balanceren | Optioneel: alleen bij omvallen. Extension point: buiten veilige grens. |
| UC05 Gezichtsuitdrukking tonen | `<<include>>` | UC07 Balansstatistieken tonen | Statistieken staan altijd naast het gezicht. |
 
Pijlrichting: bij `<<include>>` wijst de pijl naar de use case die altijd wordt uitgevoerd. Bij `<<extend>>` wijst de pijl naar de basis-use case.
 
De Gebruiker is alleen verbonden met de top van de twee hiërarchieën (UC01 en UC05). UC02, UC03, UC04, UC06 en UC07 hebben geen eigen associatie met de actor: die loopt via de hiërarchie. UC03 en UC05 hebben geen use case-relatie met elkaar: ze lopen via een bericht tussen twee subsystemen (proxy-objecten in de object models).
 
---
 
## Notities voor latere hoofdstukken (horen niet in de use cases zelf)
 
### Voorgestelde control-objecten (consistente naamgeving voor object model, klassediagram en STD)
 
| Use case | Control-object | Subsysteem |
| :--- | :--- | :--- |
| UC01 Opstarten | OpstartControl | Besturing |
| UC02 Balanceren | BalanceerControl | Besturing |
| UC03 Balansgegevens versturen | VerstuurControl (met CydProxy als boundary) | Besturing |
| UC04 Omvallen afhandelen | OmvalControl | Besturing |
| UC05 Gezichtsuitdrukking tonen | GezichtControl (met RobotProxy als boundary) | CYD |
| UC06 Balansstatistieken bijhouden | StatistiekControl | Besturing |
| UC07 Balansstatistieken tonen | StatistiekWeergaveControl | CYD |
 
### Bijbehorende requirements (nog uit te werken, waarden zijn voorstellen tot na tuning en testen)
 
| ID | Naam | Omschrijving |
| :--- | :--- | :--- |
| C01 | Aparte voedingsschakelaars | Motoren, besturing en CYD hebben elk een eigen schakelaar (vervangt de vroegere use case Uitschakelen). |
| C02 | Accu wordt niet gemeten | De acculading wordt niet gemeten. Er is geen accupercentage en geen weigering van starten bij een lage acculading. |
| NF01 | Regelperiode balanceren | Voorstel: elke 5 tot 10 ms. |
| NF02 | Verstuurperiode balansgegevens | Voorstel: elke 20 tot 50 ms. |
| NF03 | Verouderde gegevens | Voorstel: langer dan 500 ms geen geldig bericht. |
| NF04 | Veilige grens en startgrens | Voorstel: veilige grens ±35°, startgrens ±5°. |
| NF05 | Stoptijd bij omvallen | Voorstel: de wielen stoppen binnen 50 ms nadat de veilige grens is overschreden. |
 