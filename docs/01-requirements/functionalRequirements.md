# Functionele requirements

De requirements zijn afgeleid van de usecases. Iedere requirement heeft een
uniek ID, zodat in het ontwerp en de code naar de requirement verwezen kan
worden. De kolom **Bron** geeft aan uit welke usecase de requirement komt. De
prioriteit is bepaald met MoSCoW:

- **Must:** noodzakelijk; zonder deze requirement werkt de game niet of
  voldoet hij niet aan de opdracht.
- **Should:** belangrijk, maar de game is ook zonder speelbaar.
- **Could:** wenselijk als er tijd over is.
- **Would:** wordt in dit project niet gerealiseerd, maar is een mogelijke
  uitbreiding.

Getallen die zijn gemarkeerd als *(voorlopig)* zijn streefwaarden. De
definitieve waarden worden na testen vastgelegd in de
[Ontwerpparameters](#ontwerpparameters).

#### ESP32-S3 Super Mini

| **ID** | **Naam** | **Omschrijving** | **Rationale** | **Prioriteit** | **Bron** |
| :--- | :--- | :--- | :--- | :--- | :--- |
| FR-01 | Initialiseren | Na het inschakelen bepaalt de ESP32-S3 het nulpunt van de balansmeting zodra de robot rechtop en stil staat. Tot dat moment blijven de motoren stil. | Zonder nulpunt balanceert de robot schuin. De robot mag niet gaan rijden voordat de regeling klaar is. | Must | UC-S3-01 |
| FR-02 | Balanceren | De ESP32-S3 berekent uit de MPU6050-metingen een balanswaarde en houdt de robot met een PID-regeling rechtop. Het motorcommando blijft binnen de maximale motoruitgang. | Dit is de kernfunctie van de robot: zonder balansregeling is er geen spel. | Must | UC-S3-02 |
| FR-03 | Balansverlies detecteren | Ligt de balanswaarde langer dan de bevestigingstijd buiten de balansverliesgrens, dan stelt de ESP32-S3 een balansverlies vast en stopt hij de motoren. | Een balansverlies levert de speler een punt op. Een omgevallen robot met draaiende wielen is onveilig. | Must | UC-S3-03 |
| FR-04 | Valteller bijhouden | De ESP32-S3 verhoogt bij ieder balansverlies een valteller met precies één. | De valteller is de bron van de score. | Must | UC-S3-03 |
| FR-05 | Hervatten na balansverlies | Zet de speler de robot rechtop, dan gaat de robot weer balanceren zodra hij binnen de herstartgrens en stil genoeg staat. | Zonder deze functie zou de robot na iedere val opnieuw opgestart moeten worden. | Must | UC-S3-03 |
| FR-06 | Balansgegevens versturen | De ESP32-S3 verstuurt periodiek via ESP-NOW de balanswaarde, bewegingsrichting, status en valteller naar de CYD. | De CYD heeft deze gegevens nodig voor het gezicht en de score. | Must | UC-S3-04 |
| FR-07 | Fouttoestand afhandelen | Bij een sensor-, initialisatie- of regelingsfout zet de ESP32-S3 de motoren stil totdat hij opnieuw wordt opgestart. | Met onbetrouwbare meetdata kan de robot niet veilig geregeld worden. | Must | UC-S3-05 |

#### CYD ESP32

| **ID** | **Naam** | **Omschrijving** | **Rationale** | **Prioriteit** | **Bron** |
| :--- | :--- | :--- | :--- | :--- | :--- |
| FR-08 | Gezichtsexpressie tonen | De CYD toont een gezicht met expressieve ogen. De expressie hangt af van de status en de balansafwijking in het laatst ontvangen geldige bericht. | Het gezicht geeft de robot karakter en laat zien hoe de robot ervoor staat. | Must | UC-CYD-01 |
| FR-09 | Score bijhouden | De CYD geeft de speler één punt per balansverlies, op basis van de ontvangen valteller. | Dit is de spelregel van de game. | Must | UC-CYD-02 |
| FR-10 | Score tonen | De CYD toont de score onderaan het scherm, onder het gezicht. | De opdracht vraagt om een lokaal scoreboard op het display. | Must | UC-CYD-02 |
| FR-11 | Communicatieverlies tonen | Ontvangt de CYD binnen de communicatietimeout geen geldig bericht, dan toont hij dat de gegevens niet actueel zijn. | De speler moet kunnen zien dat het gezicht niet meer klopt. | Should | UC-CYD-03 |