# De weg naar het eindproduct

## Het idee

Op het moment dat het individuele game project geintroduceerd werd in de klas, zaten wij na te denken over mogelijk uitvoeringen. Het leek ons leuk om het zo veel mogelijk uit te breiden, zodat je niet alleen de CYD hebt als hardware.

We kwamen met het idee om een robot te bouwen, omdat we allebei robotica leuk vinden. We begonnen al gauw met het combineren van ideeen. Daaruit kwamen we op het idee om twee zelf balancerende robotten te maken die met elkaar lasergamen en de CYD als gezichtje en scorebord.

We hebben vervolgens toestemming gekregen van Teun Pieter Modderman om het project samen uit te voeren.

## Afbakening

We merkte snel dat we iets te enthousiast waren met onze ideeen en zoals gewoonlijk teveel hooi op onze vork namen. Zo zagen we dat wat er van ons werd verwacht qua inlevering, niet haalbaar was in combinatie met ander schoolwerk en projecten.

We hebben onze MVP aangepast en een paar requirements verwijderd. We hebben een aantal sensoren weggelaten, zoals de laser module en afstandssensor. Het doel was nu om alleen een zelf balanserende robot te maken met de CYD. We hebben afgesproken dat zodra de deadline verstreken is, dat we naar het originele idee toe werken.

## Hardwarekeuze en voeding

Door middel van theorievideo's zag ik dat we een mpu6050 nodig hadden. Deze hebben we gelukkig in het lab.

Daarnaast hebben we gebruik gemaakt van een h brug voor het besturen van de motoren.
We hadden voorheen de L298N h-brug gebruikt. Uiteindelijk hebben we gekozen voor de ZK-5AD. Deze h-brug werkt met MOSFETs en heeft veel minder spanningsverlies dan de L298N.

In eerste instantie gebruikten we lipo accus als voeding. Het voordeel was dat deze lipos licht qua gewicht zijn. In de praktijk vond ik dat onhandig, want elke lipo had een eigen charging module nodig, wat ik onhandig vond. Ik zocht iets wat gebruiksvriendelijker was. Daarnaast was als tip meegegeven om de robot was zwaarder te maken, zodat die beter balanseert. Dus we hoefte niet enorm rekening te houden met lichte onderdelen.

Ik had daarna gekeken naar Li-ion 18650-batterijen. Die waren in het lab niet meer op voorraad. Uiteindelijk heb ik oplaadbare 18650 Li-ion-batterijen (3,7 V, 2500 mAh) gekregen van Harm Snippe met een ingebouwde USB-C-aansluiting. Elke batterij kan daardoor direct met een gewone USB-C-lader worden opgeladen, zonder charging modules.

Eerst wilde ik drie van deze batterijen in serie gebruiken (11,1 V), omdat ik dacht dat alles daarmee gevoed kon worden. Later kwam ik erachter dat de motoren een hogere spanning nodig hebben dan de microcontrollers en de sensor. Daarom hebben we de voeding opgesplitst in twee pakketten van elk twee batterijen in serie (7,4 V per pakket):

- Pakket 1 voedt via een step-down converter van 5 V de CYD, de ESP32-S3 SuperMini en de MPU6050.

- Pakket 2 voedt via een step-down converter van 6 V de ZK-5AD H-brug en de motoren.

Ik kon ook met 3 batterijen twee aparte spanningen maken met 2 stepdowns aan dezelfde accu, maar 2 aparte accu's was in theorie beter omdat de stroompieken zo gescheiden zijn.

Als motoren hebben we de tt motoren in het lab gebruikt. Dit was het handigst en meest toegankelijk. (als er 12v dc motoren waren hadden we een beest robot gemaakt)

Als laatste hebben we gebruik gemaakt van een perfboard ipv een breadboard. Dit bespaarde ons ruimte en zo lukte het om alle componenten in de print te krijgen.
Ik wist voorheen niet hoe je gebruik maakt van een perfboard, maar door wat uitleg van Yunus klikte het. Ook hebben we wat tips gekregen van Stephan (docent) bij het gebruik van een perfboard. Hij liet ons makkelijkere manieren zien van verbindingen maken met soldeer tin. Een daarvan was om het bordje schuin te houden zodat die door middel van zwaartekracht al naar beneden vloeit. Of om de wire langer te knippen en al plat te drukken op het bordje.

---

Dit was een leuke leerprocess en ik kijk ernaar uit om deze robot uit te breiden/verbeteren in de toekomst.