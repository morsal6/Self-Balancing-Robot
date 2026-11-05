# Usecasebeschrijvingen zelfbalancerende robot

## Systeemoverzicht

De robot bestaat uit twee afzonderlijke subsystemen:

- De **ESP32-S3 Super Mini** leest de MPU6050 uit, voert de PID-regeling uit,
  stuurt via de ZK5AD-h-brug de twee TT130-motoren aan en stelt vast wanneer de
  robot zijn balans verliest.
- De **CYD ESP32** ontvangt balansgegevens via ESP-NOW en toont een gezicht met
  expressieve ogen en onderaan het scherm een scoreboard.

### Spelregel

De speler duwt de robot. Is de duw hard genoeg om de robot zijn balans te laten
verliezen, dan krijgt de speler één punt. Na een balansverlies stoppen de
motoren. De speler zet de robot weer rechtop, waarna de robot opnieuw gaat
balanceren en een volgende ronde kan beginnen. De score staat onderaan het
scherm van de CYD, het gezicht staat daarboven.

### Hardwarevarianten

Er zijn twee varianten, die alleen in de hardware verschillen. De software en
de usecases zijn voor beide varianten gelijk.

| | **Variant A** | **Variant B** |
| :--- | :--- | :--- |
| **Accu** | 4 Li-ion-cellen in battery shells | 3 Li-ion-cellen in battery shells |
| **Motorvoeding** | Via step-down converter naar 5 V | Via step-down converter naar 5 V |

De voeding is functioneel opgedeeld in drie delen, elk met een eigen
schakelaar:

- de **ESP32-S3-voeding** voedt de ESP32-S3 en de MPU6050;
- de **CYD-voeding** voedt alleen de CYD;
- de **motorvoeding** voedt alleen de vermogenszijde van de ZK5AD-h-brug.

Voedingsschakelaars zijn geen softwarematige start-, stop- of
noodstopfunctie.

### Robotstatus

De ESP32-S3 bevindt zich altijd in één van de volgende statussen. Deze status
wordt meegestuurd naar de CYD.

| **Status** | **Betekenis** |
| :--- | :--- |
| `KALIBREREN` | De ESP32-S3 initialiseert en bepaalt het nulpunt. Motoren staan stil. |
| `BALANCEREN` | De PID-regeling houdt de robot rechtop. |
| `BALANSVERLIES` | De robot is omgevallen. Motoren staan stil en de robot wacht tot hij rechtop wordt gezet. |
| `FOUT` | Er is een sensor-, initialisatie- of regelingsfout. Motoren staan stil tot een herstart. |

## ESP32-S3 Super Mini

| **[UC-S3-01] Initialiseren** | |
| :--- | :--- |
| **Actor** | Gebruiker, MPU6050, ZK5AD |
| **Beschrijving** | Na het inschakelen van de ESP32-S3-voeding worden de sensor, de motoruitgangen, de ESP-NOW-verbinding en de balansregeling gereedgemaakt. De motoren blijven stilstaan totdat geldige metingen beschikbaar zijn en het nulpunt is bepaald. |
| **Pre-condities** | De ESP32-S3-voeding is ingeschakeld. Hierdoor zijn de ESP32-S3 en de MPU6050 actief. De gebruiker houdt de robot rechtop en voldoende stil om te kunnen kalibreren. De motorvoeding hoeft nog niet ingeschakeld te zijn. |
| **Scenario** | 1. De ESP32-S3 start de software en zet de status op `KALIBREREN`.<br>2. Er wordt een nieuw sessie-ID gegenereerd en de valteller wordt op nul gezet.<br>3. De motoruitgangen worden geïnitialiseerd met een nulcommando.<br>4. De communicatie met de MPU6050 wordt gestart.<br>5. De ESP-NOW-verbinding met de CYD wordt geconfigureerd.<br>6. Er wordt gecontroleerd of de MPU6050 geldige metingen levert.<br>7. Zodra de robot stil genoeg staat, wordt het nulpunt van de balansmeting bepaald.<br>8. De PID-regeling wordt geïnitialiseerd met de ingestelde parameters.<br>9. De status wordt op `BALANCEREN` gezet en de balansregeling (UC-S3-02) wordt gestart. |
| **Invariant** | Zolang de initialisatie niet is voltooid, blijven de motorcommando's nul. |
| **Post-condities** | De ESP32-S3 beschikt over een geldig sensorreferentiepunt, een sessie-ID en een valteller van nul, en kan de balansregeling uitvoeren. |
| **Uitzonderingen** | • **De MPU6050 reageert niet of levert ongeldige metingen:** de motorcommando's blijven nul en de fouttoestand (UC-S3-05) wordt gestart.<br>• **De robot beweegt tijdens de kalibratie:** de kalibratie wordt afgebroken en opnieuw uitgevoerd zodra de robot stil genoeg staat.<br>• **De motoruitgangen kunnen niet veilig worden geïnitialiseerd:** de motorcommando's blijven nul en de fouttoestand (UC-S3-05) wordt gestart.<br>• **ESP-NOW kan niet worden geconfigureerd:** de initialisatie gaat door; de robot kan balanceren zonder de CYD (zie UC-S3-04). |

| **[UC-S3-02] Balanceren** | |
| :--- | :--- |
| **Actor** | MPU6050, ZK5AD, TT130-motoren |
| **Beschrijving** | Tijdens iedere regelcyclus worden de accelerometer- en gyroscoopmetingen verwerkt tot een balanswaarde. De PID-regeling gebruikt de afwijking ten opzichte van het nulpunt om de linker- en rechtermotor aan te sturen en de robot rechtop te houden. |
| **Pre-condities** | De initialisatie (UC-S3-01) is succesvol afgerond, of de robot is na een balansverlies weer rechtop gezet (UC-S3-03). De status is `BALANCEREN`. De MPU6050 levert geldige metingen. De motorvoeding is ingeschakeld, zodat de ZK5AD vermogensspanning ontvangt. |
| **Scenario** | 1. De actuele accelerometer- en gyroscoopmetingen worden uitgelezen.<br>2. De metingen worden gefilterd en gecombineerd tot een balanswaarde.<br>3. Er wordt gecontroleerd of de balanswaarde binnen de balansverliesgrens ligt.<br>4. De afwijking ten opzichte van het nulpunt wordt bepaald.<br>5. De proportionele, integrale en differentiële PID-componenten worden berekend.<br>6. De componenten worden gecombineerd tot een correctiecommando.<br>7. Het correctiecommando wordt begrensd tot het toegestane bereik.<br>8. De richting en snelheid van de linker- en rechtermotor worden bepaald.<br>9. De ZK5AD stuurt de TT130-motoren aan.<br>10. De actuele balanswaarde en status worden beschikbaar gemaakt voor UC-S3-04.<br>11. De stappen worden periodiek herhaald zolang de status `BALANCEREN` is. |
| **Invariant** | De PID-uitgang en de motorcommando's blijven binnen de ingestelde veiligheidsgrenzen. |
| **Post-condities** | De motoren voeren het meest recente geldige correctiecommando uit. |
| **Uitzonderingen** | • **Een sensorwaarde is ongeldig:** er wordt geen nieuwe correctie toegepast en de fouttoestand (UC-S3-05) wordt gestart.<br>• **De balanswaarde ligt buiten de balansverliesgrens:** er wordt geen correctie toegepast en balansverlies afhandelen (UC-S3-03) wordt gestart.<br>• **De motorvoeding wordt uitgeschakeld:** de ZK5AD en de motorvermogenszijde vallen spanningsloos; de ESP32-S3 blijft metingen verwerken en de balansgegevens blijven aan UC-S3-04 beschikbaar. |

| **[UC-S3-03] Balansverlies afhandelen** | |
| :--- | :--- |
| **Actor** | Gebruiker, MPU6050, ZK5AD, TT130-motoren |
| **Beschrijving** | Wanneer de kantelhoek de balansverliesgrens overschrijdt, kan de robot zich niet meer herstellen en is hij omgevallen. De motoren worden gestopt, de valteller wordt met één verhoogd en het balansverlies wordt aan de CYD doorgegeven. Nadat de gebruiker de robot rechtop heeft gezet, hervat de robot het balanceren. |
| **Pre-condities** | De balansregeling (UC-S3-02) is actief en de status is `BALANCEREN`. |
| **Scenario** | 1. De balanswaarde ligt langer dan de bevestigingstijd buiten de balansverliesgrens.<br>2. Het balansverlies wordt vastgesteld en de status wordt op `BALANSVERLIES` gezet.<br>3. Beide motorcommando's worden op nul gezet.<br>4. De integrale component van de PID-regeling wordt gereset.<br>5. De valteller wordt met één verhoogd.<br>6. De status en de valteller worden via UC-S3-04 naar de CYD gestuurd.<br>7. De gebruiker zet de robot rechtop.<br>8. De ESP32-S3 controleert of de balanswaarde binnen de herstartgrens ligt en de robot gedurende de hersteltijd stil genoeg staat.<br>9. De PID-regeling wordt opnieuw geïnitialiseerd.<br>10. De status wordt op `BALANCEREN` gezet en de balansregeling (UC-S3-02) wordt hervat. |
| **Invariant** | Eén balansverlies verhoogt de valteller precies één keer. Zolang de status `BALANSVERLIES` is, blijven de motorcommando's nul. |
| **Post-condities** | De valteller bevat het totaal aantal balansverliezen sinds het opstarten van de ESP32-S3. De robot balanceert opnieuw. |
| **Uitzonderingen** | • **De balanswaarde komt kort terug binnen de balansverliesgrens, maar niet binnen de herstartgrens:** de status blijft `BALANSVERLIES` en de valteller wordt niet opnieuw verhoogd. De herstartgrens ligt ruim binnen de balansverliesgrens, zodat een wiebelende robot niet dubbel telt.<br>• **De robot wordt niet rechtop gezet:** de status blijft `BALANSVERLIES`, de motoren blijven stil en de ESP32-S3 blijft de balanswaarde controleren.<br>• **De robot beweegt tijdens het rechtop zetten:** de hersteltijd begint opnieuw zodra de robot stil genoeg staat.<br>• **Een sensorwaarde is ongeldig:** de fouttoestand (UC-S3-05) wordt gestart. |

| **[UC-S3-04] Versturen** | |
| :--- | :--- |
| **Actor** | CYD ESP32 via ESP-NOW |
| **Beschrijving** | De actuele balanswaarde, bewegingsrichting, status en valteller worden periodiek via ESP-NOW naar de CYD ESP32 gestuurd. De gegevens worden gebruikt voor de gezichtsexpressie en het scoreboard. |
| **Pre-condities** | De ESP32-S3 beschikt over actuele balansinformatie en een status. De ESP-NOW-communicatie is geconfigureerd. |
| **Scenario** | 1. De actuele balanswaarde, bewegingsrichting, status en valteller worden gelezen.<br>2. Deze gegevens worden samen met het sessie-ID in een bericht geplaatst.<br>3. Een volgnummer wordt aan het bericht toegevoegd.<br>4. Het bericht wordt via ESP-NOW verzonden.<br>5. De CYD ESP32 controleert het bericht.<br>6. Een geldig bericht wordt beschikbaar gemaakt voor UC-CYD-01 en UC-CYD-02.<br>7. De verzending wordt periodiek herhaald, ook als de status `BALANSVERLIES` of `FOUT` is. |
| **Invariant** | Een storing in ESP-NOW blokkeert de balansregeling en motorsturing niet. |
| **Post-condities** | De CYD ESP32 beschikt over nieuwe geldige balansgegevens, of kan vaststellen dat er geen actuele gegevens zijn. |
| **Uitzonderingen** | • **De CYD ESP32 is niet bereikbaar:** de ESP32-S3 blijft zelfstandig werken en probeert bij de volgende cyclus opnieuw te verzenden.<br>• **Het bericht kan niet worden verzonden:** de balansregeling blijft actief en de verzendfout wordt niet als een geldig ontvangstbericht behandeld.<br>• **Een bericht met een balansverlies gaat verloren:** het volgende bericht bevat de bijgewerkte valteller, zodat de CYD het gemiste balansverlies alsnog kan tellen. |

| **[UC-S3-05] Fouttoestand afhandelen** | |
| :--- | :--- |
| **Actor** | MPU6050, ZK5AD, ESP32-S3 |
| **Beschrijving** | Wanneer sensorwaarden, regelberekeningen of motoruitgangen niet betrouwbaar zijn, worden de motorcommando's naar de veilige waarde gebracht. De fouttoestand wordt, als dat mogelijk is, naar de CYD ESP32 gestuurd. Een fouttoestand is iets anders dan een balansverlies: een balansverlies hoort bij het spel, een fout niet. |
| **Pre-condities** | De ESP32-S3 is gestart of probeert te starten. |
| **Scenario** | 1. Een sensor-, initialisatie- of regelingsfout wordt vastgesteld.<br>2. De balansinformatie wordt als ongeldig gemarkeerd.<br>3. Beide motorcommando's worden op nul gezet.<br>4. De status wordt op `FOUT` gezet.<br>5. De foutstatus wordt via UC-S3-04 naar de CYD ESP32 gestuurd.<br>6. De fouttoestand blijft actief totdat de ESP32-S3 opnieuw wordt opgestart. |
| **Invariant** | Ongeldige meetdata wordt nooit als geldige balanswaarde gebruikt. Een fout verhoogt de valteller niet. |
| **Post-condities** | De motoren worden niet meer aangestuurd met onbetrouwbare regeldata. |
| **Uitzonderingen** | • **De communicatielink naar de CYD ESP32 werkt niet:** de motoruitgangen blijven veilig en de fouttoestand blijft actief.<br>• **Meerdere fouten ontstaan tegelijk:** de foutreactie met het grootste veiligheidsrisico heeft voorrang. |

## CYD ESP32

| **[UC-CYD-01] Tonen** | |
| :--- | :--- |
| **Actor** | ESP32-S3 via ESP-NOW, gebruiker |
| **Beschrijving** | De CYD ESP32 ontvangt geldige berichten en vertaalt de status, balansafwijking en bewegingsrichting naar expressieve ogen en een gezicht. |
| **Pre-condities** | De CYD-voeding is ingeschakeld. Het scherm en de ESP-NOW-ontvanger zijn geïnitialiseerd. |
| **Scenario** | 1. Een ESP-NOW-bericht wordt ontvangen.<br>2. Het formaat, de bron en het volgnummer van het bericht worden gecontroleerd.<br>3. De status, balansafwijking en bewegingsrichting worden gelezen.<br>4. Een gezichtsexpressie wordt gekozen:<br>&nbsp;&nbsp;• `KALIBREREN`: opstart- of wachtexpressie;<br>&nbsp;&nbsp;• `BALANCEREN`: expressie die past bij de balansafwijking en bewegingsrichting;<br>&nbsp;&nbsp;• `BALANSVERLIES`: gevallen-expressie;<br>&nbsp;&nbsp;• `FOUT`: foutexpressie.<br>5. De positie, richting of vorm van de ogen wordt aangepast.<br>6. Het gezicht wordt in het bovenste deel van het scherm getekend.<br>7. De valteller en het sessie-ID worden doorgegeven aan UC-CYD-02.<br>8. De weergave wordt bij nieuwe geldige berichten bijgewerkt. |
| **Invariant** | De gezichtsexpressie is gebaseerd op het laatst ontvangen geldige bericht. |
| **Post-condities** | Het scherm toont een gezicht dat de laatst bekende toestand van de robot weergeeft. |
| **Uitzonderingen** | • **Het bericht is ongeldig of verouderd:** het bericht wordt genegeerd en de vorige geldige toestand blijft behouden totdat de communicatietimeout verstrijkt.<br>• **De balanswaarde valt buiten het expressiebereik:** de waarde wordt begrensd tot de beschikbare gezichtstoestanden.<br>• **Er is geen geldig bericht binnen de timeout:** communicatieverlies tonen (UC-CYD-03) wordt gestart. |

| **[UC-CYD-02] Score bijhouden** | |
| :--- | :--- |
| **Actor** | ESP32-S3 via ESP-NOW, gebruiker |
| **Beschrijving** | De CYD ESP32 houdt het aantal punten bij. Iedere keer dat de robot zijn balans verliest, krijgt de speler één punt. De CYD gebruikt hiervoor de valteller van de ESP32-S3, zodat een gemist bericht geen punt kost en een herhaald bericht geen extra punt oplevert. De actuele score wordt onderaan het scherm getoond. |
| **Pre-condities** | De CYD-voeding is ingeschakeld en er is een geldig bericht ontvangen. De score is bij het opstarten van de CYD op nul gezet. |
| **Scenario** | 1. De valteller en het sessie-ID worden uit het geldige bericht gelezen.<br>2. Bij het eerste geldige bericht na het opstarten van de CYD worden de valteller en het sessie-ID als referentie opgeslagen; de score wordt niet verhoogd.<br>3. Bij volgende berichten wordt gecontroleerd of het sessie-ID gelijk is aan de referentie.<br>4. Het verschil tussen de nieuwe valteller en de referentie-valteller wordt bepaald.<br>5. De score wordt met dit verschil verhoogd.<br>6. De nieuwe valteller wordt de referentie-valteller.<br>7. De score wordt onderaan het scherm getoond.<br>8. De score blijft bewaard zolang de CYD ingeschakeld blijft. |
| **Invariant** | De score kan tijdens één voedingssessie van de CYD alleen gelijk blijven of toenemen. Ieder balansverlies levert precies één punt op. |
| **Post-condities** | Het scoreboard toont het aantal balansverliezen tijdens de huidige voedingssessie van de CYD. |
| **Uitzonderingen** | • **Er is geen geldig bericht:** de score wordt niet verhoogd op basis van ontbrekende data.<br>• **Er zijn berichten gemist:** het verschil in valteller is groter dan één en alle gemiste balansverliezen worden alsnog geteld.<br>• **Het sessie-ID is veranderd (de ESP32-S3 is herstart):** de score blijft staan, wordt verhoogd met de nieuwe valteller (die na een herstart bij nul begint) en het nieuwe sessie-ID en de nieuwe valteller worden de referentie.<br>• **De maximale score is bereikt:** de score blijft op de maximale waarde staan.<br>• **De CYD wordt uitgeschakeld en opnieuw ingeschakeld:** de score wordt op nul gezet. |

| **[UC-CYD-03] Communicatieverlies tonen** | |
| :--- | :--- |
| **Actor** | ESP-NOW-verbinding, gebruiker |
| **Beschrijving** | Wanneer de CYD ESP32 geen geldig bericht meer ontvangt, maakt het scherm zichtbaar dat de getoonde informatie niet actueel is. |
| **Pre-condities** | De CYD ESP32 is geïnitialiseerd en wacht op of heeft eerder een geldig bericht ontvangen. |
| **Scenario** | 1. Er verstrijkt de ingestelde timeout zonder geldig bericht.<br>2. De laatst bekende balansinformatie wordt als niet-actueel gemarkeerd.<br>3. Een communicatieverlies- of wachtexpressie wordt getoond.<br>4. De laatst bekende score blijft onderaan zichtbaar, maar wordt niet verhoogd zolang er geen geldig bericht is.<br>5. Bij een nieuw geldig bericht wordt UC-CYD-01 hervat. |
| **Invariant** | Oude balansdata wordt nooit zonder waarschuwing als actuele data getoond. |
| **Post-condities** | De gebruiker kan onderscheid maken tussen actuele robotdata en een verbroken verbinding. |
| **Uitzonderingen** | • **De verbinding wordt hersteld:** de nieuwe geldige gegevens worden gebruikt voor de gezichtsexpressie en score; balansverliezen tijdens de onderbreking worden via de valteller alsnog geteld.<br>• **De verbinding blijft verbroken:** de communicatieverlies-expressie blijft zichtbaar. |

## Relaties tussen de usecases

- UC-S3-01 `include` UC-S3-02.
- UC-S3-02 `include` UC-S3-04.
- UC-S3-03 `extend` UC-S3-02 wanneer de balanswaarde buiten de balansverliesgrens ligt.
- UC-S3-03 `include` UC-S3-04.
- UC-S3-05 kan optreden als uitzondering tijdens UC-S3-01, UC-S3-02 en UC-S3-03.
- UC-CYD-01 `include` UC-CYD-02.
- UC-CYD-03 `extend` UC-CYD-01 wanneer de communicatietimeout verstrijkt.

## Nog vast te leggen ontwerpwaarden

- De MPU6050-balansas en de tekenconventie van de balanswaarde.
- De PID-parameters en de maximale motoruitgang.
- De regelperiode en ESP-NOW-verzendfrequentie.
- Het ESP-NOW-berichtformaat (velden en datatypes).
- De communicatietimeout.
- De balansverliesgrens en de bevestigingstijd.
- De herstartgrens en de hersteltijd na een balansverlies.
- De maximumscore.
- De koppeling tussen status, balanswaarde en gezichtsexpressie.
- De laad- en beveiligingsarchitectuur per variant (4 Li-ion-cellen in variant A, 3 Li-ion-cellen in variant B).
