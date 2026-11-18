# Usecase beschrijvingen 

## ESP32-S3 Super Mini

Zie het overzichtsdocument voor de werking van de hele robot, de voeding en de
inhoud van de ESP-NOW-berichten.

| **[UC-S3-00] Robot besturen** | |
| :--- | :--- |
| **Actor** | Gebruiker |
| **Beschrijving** | Bepaalt wanneer de robot initialiseert, balanceert en een balansverlies afhandelt, en wat er gebeurt bij een fout. Houdt UC-S3-04 op de hoogte van de fase waarin de robot zich bevindt. UC-S3-04 Versturen draait parallel aan deze use case. |
| **Scenario** | 1. Aan UC-S3-04 wordt doorgegeven dat de robot in de fase `KALIBREREN` is.<br>2. Voer UC-S3-01 Initialiseren uit.<br>3. Aan UC-S3-04 wordt doorgegeven dat de robot in de fase `BALANCEREN` is.<br>4. Voer UC-S3-02 Balanceren uit.<br>5. Aan UC-S3-04 wordt doorgegeven dat de robot in de fase `BALANSVERLIES` is.<br>6. Voer UC-S3-03 Balansverlies afhandelen uit.<br>7. Goto 3. |
| **Uitzonderingen** | - **UC-S3-01, UC-S3-02 of UC-S3-03 stelt een fout vast:** aan UC-S3-04 wordt doorgegeven dat de robot in de fase `FOUT` is, en UC-S3-05 Fouttoestand afhandelen wordt uitgevoerd. |

| **[UC-S3-01] Initialiseren** | |
| :--- | :--- |
| **Actor** | Gebruiker, MPU6050, ZK5AD |
| **Beschrijving** | Maakt de sensor, de motoruitgangen, de ESP-NOW-verbinding en de balansregeling gereed, en bepaalt het nulpunt van de balansmeting terwijl de gebruiker de robot rechtop en stil houdt. |
| **Pre-condities** | UC-S3-00 voert deze use case uit, direct na het opstarten van de ESP32-S3. De gebruiker houdt de robot rechtop en stil. |
| **Scenario** | 1. Er wordt een nieuw sessie-ID gegenereerd en de valteller wordt op nul gezet.<br>2. De motoruitgangen worden geïnitialiseerd met een nulcommando.<br>3. De communicatie met de MPU6050 wordt gestart.<br>4. De ESP-NOW-verbinding met de CYD wordt geconfigureerd.<br>5. Wacht op een nieuwe meting van de MPU6050.<br>6. Is de meting ongeldig, zie de uitzondering *De MPU6050 levert een ongeldige meting*.<br>7. Beweegt de robot, dan begint de kalibratietijd opnieuw: goto 5.<br>8. Staat de robot nog niet gedurende de kalibratietijd stil: goto 5.<br>9. Het nulpunt van de balansmeting wordt bepaald.<br>10. De PID-regeling wordt geïnitialiseerd met de ingestelde parameters. |
| **Invariant** | Zolang de initialisatie niet is voltooid, blijven de motorcommando's nul. |
| **Post-condities** | De ESP32-S3 beschikt over een geldig nulpunt, een sessie-ID en een valteller van nul. Terug naar UC-S3-00. |
| **Uitzonderingen** | - **De MPU6050 reageert niet of levert een ongeldige meting:** de motorcommando's blijven nul en er wordt een sensorfout aan UC-S3-00 gemeld.<br>- **De motoruitgangen kunnen niet veilig worden geïnitialiseerd:** de motorcommando's blijven nul en er wordt een initialisatiefout aan UC-S3-00 gemeld.<br>- **ESP-NOW kan niet worden geconfigureerd:** de initialisatie gaat door; de robot kan balanceren zonder de CYD. |

| **[UC-S3-02] Balanceren** | |
| :--- | :--- |
| **Actor** | MPU6050, ZK5AD, TT130-motoren |
| **Beschrijving** | Verwerkt iedere meting tot een balanswaarde. De PID-regeling gebruikt de afwijking ten opzichte van het nulpunt om de linker- en rechtermotor aan te sturen en de robot rechtop te houden, totdat de robot zijn balans verliest. |
| **Pre-condities** | UC-S3-00 voert deze use case uit, na een geslaagde initialisatie (UC-S3-01) of nadat de robot weer rechtop is gezet (UC-S3-03). |
| **Scenario** | 1. Wacht op een nieuwe meting van de MPU6050. De MPU6050 levert iedere regelperiode een meting.<br>2. De accelerometer- en gyroscoopmetingen worden gefilterd en gecombineerd tot een balanswaarde.<br>3. Ligt de balanswaarde buiten de balansverliesgrens: goto 9.<br>4. De afwijking ten opzichte van het nulpunt wordt bepaald.<br>5. De proportionele, integrale en differentiële PID-componenten worden berekend en gecombineerd tot een correctiecommando, dat wordt begrensd tot het toegestane bereik.<br>6. De richting en snelheid van de linker- en rechtermotor worden bepaald, en de ZK5AD stuurt de TT130-motoren aan.<br>7. De actuele balanswaarde en bewegingsrichting worden aan UC-S3-04 doorgegeven.<br>8. Goto 1.<br>9. Er wordt geen correctie toegepast.<br>10. Ligt de balanswaarde nog niet gedurende de bevestigingstijd onafgebroken buiten de balansverliesgrens: goto 1.<br>11. Er is een balansverlies vastgesteld. |
| **Invariant** | De PID-uitgang en de motorcommando's blijven binnen de ingestelde veiligheidsgrenzen. |
| **Post-condities** | Er is een balansverlies vastgesteld. Terug naar UC-S3-00. |
| **Uitzonderingen** | - **Een sensorwaarde is ongeldig:** er wordt geen nieuwe correctie toegepast, beide motorcommando's worden op nul gezet en er wordt een sensorfout aan UC-S3-00 gemeld. |

| **[UC-S3-03] Balansverlies afhandelen** | |
| :--- | :--- |
| **Actor** | Gebruiker, MPU6050, ZK5AD, TT130-motoren |
| **Beschrijving** | Na een balansverlies worden de motoren gestopt en wordt de valteller met één verhoogd. Daarna wacht de robot tot de gebruiker hem weer rechtop zet. De herstartgrens ligt ruim binnen de balansverliesgrens, zodat een wiebelende robot niet te vroeg herstart. |
| **Pre-condities** | UC-S3-00 voert deze use case uit, nadat UC-S3-02 een balansverlies heeft vastgesteld. |
| **Scenario** | 1. Beide motorcommando's worden op nul gezet.<br>2. De integrale component van de PID-regeling wordt gereset.<br>3. De valteller wordt met één verhoogd en aan UC-S3-04 doorgegeven.<br>4. Wacht op een nieuwe meting van de MPU6050, terwijl de gebruiker de robot rechtop zet.<br>5. De metingen worden gefilterd en gecombineerd tot een balanswaarde.<br>6. Ligt de balanswaarde buiten de herstartgrens of beweegt de robot, dan begint de hersteltijd opnieuw: goto 4.<br>7. Staat de robot nog niet gedurende de hersteltijd rechtop en stil: goto 4.<br>8. De PID-regeling wordt opnieuw geïnitialiseerd. |
| **Invariant** | Een balansverlies verhoogt de valteller precies een keer. Zolang deze use case loopt, blijven de motorcommando's nul. |
| **Post-condities** | De robot staat rechtop en stil, en de PID-regeling is opnieuw geïnitialiseerd. Terug naar UC-S3-00. |
| **Uitzonderingen** | - **Een sensorwaarde is ongeldig:** er wordt een sensorfout aan UC-S3-00 gemeld. |

| **[UC-S3-04] Versturen** | |
| :--- | :--- |
| **Actor** | CYD ESP32 (via ESP-NOW) |
| **Beschrijving** | Stuurt periodiek de actuele gegevens via ESP-NOW naar de CYD, zodat die het gezicht en de score kan tonen. Draait parallel aan UC-S3-00. |
| **Scenario** | 1. Wacht tot de verzendperiode verstreken is, of tot er een nieuwe fase of valteller is doorgegeven.<br>2. De actuele balanswaarde, bewegingsrichting, fase en valteller worden gelezen.<br>3. Deze gegevens worden samen met het sessie-ID en een opgehoogd volgnummer in een bericht geplaatst.<br>4. Het bericht wordt via ESP-NOW naar de CYD verzonden.<br>5. Goto 1. |
| **Invariant** | Een storing in ESP-NOW blokkeert de balansregeling en de motorsturing niet. |
| **Uitzonderingen** | - **De CYD is niet bereikbaar of het bericht kan niet worden verzonden:** het bericht vervalt; bij de volgende verzendperiode wordt een nieuw bericht verstuurd.<br>- **Een bericht met een balansverlies gaat verloren:** het volgende bericht bevat de bijgewerkte valteller, zodat de CYD het gemiste balansverlies alsnog kan tellen. |

| **[UC-S3-05] Fouttoestand afhandelen** | |
| :--- | :--- |
| **Actor** | ZK5AD |
| **Beschrijving** | Wanneer sensorwaarden, regelberekeningen of motoruitgangen niet betrouwbaar zijn, worden de motoren veilig stilgezet. Een fouttoestand is iets anders dan een balansverlies: een balansverlies hoort bij het spel, een fout niet. |
| **Pre-condities** | UC-S3-00 voert deze use case uit, nadat UC-S3-01, UC-S3-02 of UC-S3-03 een fout heeft gemeld. |
| **Scenario** | 1. Beide motorcommando's worden op nul gezet.<br>2. De balansinformatie wordt als ongeldig gemarkeerd, zodat UC-S3-04 geen ongeldige balanswaarde verstuurt.<br>3. Wacht tot de ESP32-S3 opnieuw wordt opgestart (met de resetknop). |
| **Invariant** | Ongeldige meetdata wordt nooit als geldige balanswaarde gebruikt. Een fout verhoogt de valteller niet. |

### Relaties tussen de usecases

- UC-S3-00 `include` UC-S3-01, UC-S3-02 en UC-S3-03 (UC-S3-00 voert ze na elkaar uit).
- UC-S3-05 `extend` UC-S3-00 wanneer een uitgevoerde use case een fout meldt.
- UC-S3-04 draait parallel aan UC-S3-00 en heeft geen relatie met de andere use cases: de gegevens worden alleen doorgegeven.

### Nog vast te leggen ontwerpwaarden

- De MPU6050-balansas en de tekenconventie van de balanswaarde.
- De PID-parameters en de maximale motoruitgang.
- De regelperiode.
- De kalibratietijd (hoe lang de robot stil moet staan tijdens het initialiseren).
- De balansverliesgrens en de bevestigingstijd.
- De herstartgrens en de hersteltijd na een balansverlies.
- Wanneer de robot "beweegt" (grens voor de gyroscoopmeting).

## CYD ESP32

| **[UC-CYD-01] Tonen** | |
| :--- | :--- |
| **Actor** | ESP32-S3 (via ESP-NOW), Gebruiker |
| **Beschrijving** | De CYD ontvangt berichten van de ESP32-S3 en vertaalt de fase, de balansafwijking en de bewegingsrichting naar een gezicht met expressieve ogen. |
| **Pre-condities** | — |
| **Scenario** | 1. Het scherm en de ESP-NOW-ontvanger worden geïnitialiseerd.<br>2. Wacht op een ESP-NOW-bericht.<br>3. Het formaat, de bron en het volgnummer van het bericht worden gecontroleerd.<br>4. De fase, balansafwijking en bewegingsrichting worden gelezen.<br>5. Een gezichtsexpressie wordt gekozen:<br>&nbsp;&nbsp;- `KALIBREREN`: opstart- of wachtexpressie;<br>&nbsp;&nbsp;- `BALANCEREN`: expressie die past bij de balansafwijking en bewegingsrichting;<br>&nbsp;&nbsp;- `BALANSVERLIES`: gevallen-expressie;<br>&nbsp;&nbsp;- `FOUT`: foutexpressie.<br>6. De positie, richting of vorm van de ogen wordt aangepast.<br>7. Het gezicht wordt in het bovenste deel van het scherm getekend.<br>8. De valteller en het sessie-ID worden doorgegeven aan UC-CYD-02.<br>9. Goto 2. |
| **Uitzonderingen** | - **Het bericht is ongeldig of verouderd:** het bericht wordt genegeerd en het getoonde gezicht blijft staan: goto 2.<br>- **De balanswaarde valt buiten het expressiebereik:** de waarde wordt begrensd tot de beschikbare gezichtstoestanden.<br>- **Er is binnen de communicatietimeout geen geldig bericht ontvangen:** voer UC-CYD-03 Communicatieverlies tonen uit en wacht tot die klaar is, daarna goto 2. |

| **[UC-CYD-02] Score bijhouden** | |
| :--- | :--- |
| **Actor** | Gebruiker |
| **Beschrijving** | De CYD houdt per spel het aantal punten bij. Iedere keer dat de robot zijn balans verliest, krijgt de speler een punt. De CYD gebruikt hiervoor de valteller van de ESP32-S3, zodat een gemist bericht geen punt kost en een herhaald bericht geen extra punt oplevert. Door op het touchscreen te tikken start de gebruiker een nieuw spel. Deze use case draait parallel aan UC-CYD-01. |
| **Scenario** | 1. De score wordt op nul gezet en onderaan het scherm getoond.<br>2. Wacht tot UC-CYD-01 een valteller en sessie-ID doorgeeft, of tot de gebruiker op het touchscreen tikt.<br>3. Heeft de gebruiker op het touchscreen getikt, dan begint er een nieuw spel: de laatst ontvangen valteller wordt de referentie-valteller. Goto 1.<br>4. Is er nog geen referentie (de eerste gegevens na het opstarten van de CYD): de valteller en het sessie-ID worden als referentie opgeslagen. Goto 2.<br>5. Is het sessie-ID anders dan de referentie (de ESP32-S3 is opnieuw opgestart): de score wordt verhoogd met de nieuwe valteller, en het nieuwe sessie-ID en de nieuwe valteller worden de referentie. Goto 7.<br>6. De score wordt verhoogd met het verschil tussen de nieuwe valteller en de referentie-valteller, en de nieuwe valteller wordt de referentie-valteller.<br>7. De score wordt onderaan het scherm getoond.<br>8. Goto 2. |
| **Invariant** | Tijdens een spel kan de score alleen gelijk blijven of toenemen. Ieder balansverlies levert precies een punt op. |
| **Uitzonderingen** | - **Er zijn berichten gemist:** het verschil in valteller is groter dan een, en alle gemiste balansverliezen worden alsnog geteld.<br>- **De maximale score is bereikt:** de score blijft op de maximale waarde staan. |

| **[UC-CYD-03] Communicatieverlies tonen** | |
| :--- | :--- |
| **Actor** | Gebruiker |
| **Beschrijving** | Maakt op het scherm zichtbaar dat de getoonde informatie niet meer actueel is, omdat er geen geldige berichten meer binnenkomen. |
| **Pre-condities** | UC-CYD-01 voert deze use case uit, omdat er binnen de communicatietimeout geen geldig bericht is ontvangen. |
| **Scenario** | 1. De laatst bekende balansinformatie wordt als niet-actueel gemarkeerd.<br>2. In het bovenste deel van het scherm wordt een communicatieverlies-expressie getoond. De score onderaan blijft zichtbaar. |
| **Invariant** | Oude balansdata wordt nooit zonder waarschuwing als actuele data getoond. |
| **Post-condities** | De communicatieverlies-expressie blijft zichtbaar totdat UC-CYD-01 weer een geldig bericht ontvangt. Terug naar UC-CYD-01. |

### Relaties tussen de usecases

- UC-CYD-03 `extend` UC-CYD-01 wanneer de communicatietimeout verstrijkt (UC-CYD-01 voert UC-CYD-03 uit en wacht tot die klaar is).
- UC-CYD-01 en UC-CYD-02 draaien parallel en hebben geen relatie: UC-CYD-01 geeft alleen de valteller en het sessie-ID door.

### Nog vast te leggen ontwerpwaarden

- De communicatietimeout.
- De maximumscore.
- De koppeling tussen fase, balanswaarde en gezichtsexpressie.
- De aansluiting en uitlezing van het touchscreen (XPT2046).
