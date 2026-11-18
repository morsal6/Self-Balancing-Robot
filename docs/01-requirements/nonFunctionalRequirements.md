# Non-functionele requirements

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

| **ID** | **Naam** | **Omschrijving** | **Rationale** | **Prioriteit** |
| :--- | :--- | :--- | :--- | :--- |
| NFR-01 | Regelfrequentie | De balansregeling draait minimaal op 100 Hz *(voorlopig)*. | Bij een lagere frequentie valt de robot om voordat de correctie effect heeft. | Must |
| NFR-02 | Onafhankelijke regeling | Een storing in ESP-NOW of de CYD heeft geen invloed op de balansregeling. | De robot moet blijven balanceren, ook als de verbinding met de CYD wegvalt. | Must |
| NFR-03 | Veilige motorstand | Buiten de status `BALANCEREN` zijn de motorcommando's altijd nul. | De robot mag niet rijden tijdens kalibreren, na een val of bij een fout. | Must |
| NFR-04 | Balanceerduur | Zonder verstoring blijft de robot minimaal 60 seconden rechtop staan *(voorlopig)*. | Valt de robot vanzelf om, dan krijgt de speler een punt zonder te duwen. | Must |
| NFR-05 | Lichte duw opvangen | Een lichte duw leidt niet tot een balansverlies. | Alleen een harde duw mag een punt opleveren. | Should |
| NFR-06 | Reactietijd display | Een verandering in status of balans is binnen 200 ms zichtbaar op het scherm *(voorlopig)*. | Anders lijkt het gezicht niet bij de robot te horen. | Should |