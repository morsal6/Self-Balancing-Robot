# Usecasebeschrijvingen zelfbalancerende robot

De robot bestaat uit twee afzonderlijke subsystemen:

- De **ESP32-S3 Super Mini** leest de MPU6050 uit, voert de PID-regeling uit en
  stuurt via de ZK5AD-h-brug de twee TT130-motoren aan.
- De **CYD ESP32** ontvangt balansgegevens via ESP-NOW en toont een gezicht met
  expressieve ogen en onderaan een scoreboard.

Er zijn drie afzonderlijke voedingsschakelaars. De ESP32-S3-schakelaar voedt
de ESP32-S3 en de MPU6050. De CYD-schakelaar voedt alleen de CYD. De
motorvoedingsschakelaar voedt alleen de vermogenszijde van de ZK5AD-h-brug.
De voedingsschakelaars zijn geen softwarematige start-, stop- of noodstopfunctie.

## ESP32-S3 Super Mini

| **[UC-S3-01] Initialiseren** | |
| :--- | :--- |
| **Actor** | Gebruiker, MPU6050, ZK5AD |
| **Beschrijving** | Na het inschakelen van de ESP32-S3-voeding worden de sensor, de motoruitgangen en de balansregeling gereedgemaakt. De motoren blijven stilstaan totdat geldige metingen beschikbaar zijn en het nulpunt is bepaald. |
| **Pre-condities** | De ESP32-S3-voeding is ingeschakeld. Hierdoor zijn de ESP32-S3 en de MPU6050 actief. De robot staat voldoende stil om te kunnen kalibreren. De motorvoeding hoeft nog niet ingeschakeld te zijn. |
| **Scenario** | 1. De ESP32-S3 start de software.<br>2. De communicatie met de MPU6050 wordt gestart.<br>3. De motoruitgangen worden geïnitialiseerd met een nulcommando.<br>4. Er wordt gecontroleerd of de MPU6050 geldige metingen levert.<br>5. Zodra de robot stil genoeg staat, wordt het nulpunt van de balansmeting bepaald.<br>6. De PID-regeling wordt geïnitialiseerd met de ingestelde parameters.<br>7. De balansregeling (UC-S3-02) wordt gestart. |
| **Invariant** | Zolang de initialisatie niet is voltooid, blijven de motorcommando's nul. |
| **Post-condities** | De ESP32-S3 beschikt over een geldig sensorreferentiepunt en kan de balansregeling uitvoeren. |
| **Uitzonderingen** | • **De MPU6050 reageert niet of levert ongeldige metingen:** de motorcommando's blijven nul en de fouttoestand (UC-S3-04) wordt gestart.<br>• **De robot beweegt tijdens de kalibratie:** de kalibratie wordt afgebroken en opnieuw uitgevoerd zodra de robot stil genoeg staat.<br>• **De motoruitgangen kunnen niet veilig worden geïnitialiseerd:** de motorcommando's blijven nul en de fouttoestand (UC-S3-04) wordt gestart. |

| **[UC-S3-02] Balanceren** | |
| :--- | :--- |
| **Actor** | MPU6050, ZK5AD, TT130-motoren |
| **Beschrijving** | Tijdens iedere regelcyclus worden de accelerometer- en gyroscoopmetingen verwerkt tot een balanswaarde. De PID-regeling gebruikt de afwijking ten opzichte van het nulpunt om de linker- en rechtermotor aan te sturen en de robot rechtop te houden. |
| **Pre-condities** | De initialisatie (UC-S3-01) is succesvol afgerond. De MPU6050 levert geldige metingen. De motorvoedingsschakelaar is ingeschakeld, zodat de ZK5AD vermogensspanning ontvangt. |
| **Scenario** | 1. De actuele accelerometer- en gyroscoopmetingen worden uitgelezen.<br>2. De metingen worden gefilterd en gecombineerd tot een balanswaarde.<br>3. De afwijking ten opzichte van het nulpunt wordt bepaald.<br>4. De proportionele, integrale en differentiële PID-componenten worden berekend.<br>5. De componenten worden gecombineerd tot een correctiecommando.<br>6. Het correctiecommando wordt begrensd tot het toegestane bereik.<br>7. De richting en snelheid van de linker- en rechtermotor worden bepaald.<br>8. De ZK5AD stuurt de TT130-motoren aan.<br>9. De stappen worden periodiek herhaald zolang de besturing en motorvoeding actief zijn. |
| **Invariant** | De PID-uitgang en de motorcommando's blijven binnen de ingestelde veiligheidsgrenzen. |
| **Post-condities** | De motoren voeren het meest recente geldige correctiecommando uit. |
| **Uitzonderingen** | • **Een sensorwaarde is ongeldig:** er wordt geen nieuwe correctie toegepast en de fouttoestand (UC-S3-04) wordt gestart.<br>• **De robot helt verder dan de maximale regelbare hoek:** de motoruitgang wordt begrensd en de toestand wordt als sterk uit balans doorgegeven aan UC-S3-03.<br>• **De motorvoeding wordt uitgeschakeld:** de ZK5AD en de motorvermogenszijde vallen spanningsloos; de balansgegevens blijven, zolang de ESP32-S3 actief is, aan UC-S3-03 beschikbaar. |

| **[UC-S3-03] Versturen** | |
| :--- | :--- |
| **Actor** | CYD ESP32 via ESP-NOW |
| **Beschrijving** | De actuele balansafwijking, bewegingsrichting en relevante toestand worden periodiek via ESP-NOW naar de CYD ESP32 gestuurd. De gegevens worden gebruikt voor de gezichtsexpressie en het scoreboard. |
| **Pre-condities** | De ESP32-S3 beschikt over actuele balansinformatie. De ESP-NOW-communicatie is geconfigureerd. |
| **Scenario** | 1. De actuele balanswaarde wordt gelezen.<br>2. De balanswaarde, bewegingsrichting en status worden in een bericht geplaatst.<br>3. Een volgnummer of tijdsindicatie wordt aan het bericht toegevoegd.<br>4. Het bericht wordt via ESP-NOW verzonden.<br>5. De CYD ESP32 controleert het bericht.<br>6. Een geldig bericht wordt beschikbaar gemaakt voor UC-CYD-01 en UC-CYD-02.<br>7. De verzending wordt periodiek herhaald. |
| **Invariant** | Een storing in ESP-NOW blokkeert de balansregeling en motorsturing niet. |
| **Post-condities** | De CYD ESP32 beschikt over nieuwe geldige balansgegevens, of kan vaststellen dat er geen actuele gegevens zijn. |
| **Uitzonderingen** | • **De CYD ESP32 is niet bereikbaar:** de ESP32-S3 blijft zelfstandig balanceren en probeert bij de volgende cyclus opnieuw te verzenden.<br>• **Het bericht kan niet worden verzonden:** de balansregeling blijft actief en de verzendfout wordt niet als een geldig ontvangstbericht behandeld. |

| **[UC-S3-04] Fouttoestand afhandelen** | |
| :--- | :--- |
| **Actor** | MPU6050, ZK5AD, ESP32-S3 |
| **Beschrijving** | Wanneer sensorwaarden, regelberekeningen of motoruitgangen niet betrouwbaar zijn, worden de motorcommando's naar de veilige waarde gebracht. De fouttoestand wordt, als dat mogelijk is, naar de CYD ESP32 gestuurd. |
| **Pre-condities** | De ESP32-S3 is gestart of probeert te starten. |
| **Scenario** | 1. Een sensor-, initialisatie- of regelingsfout wordt vastgesteld.<br>2. De balansinformatie wordt als ongeldig gemarkeerd.<br>3. Beide motorcommando's worden op nul of op de vooraf bepaalde veilige waarde gezet.<br>4. De foutstatus wordt via UC-S3-03 naar de CYD ESP32 gestuurd.<br>5. De fouttoestand blijft actief totdat de ESP32-S3 opnieuw wordt opgestart. |
| **Invariant** | Ongeldige meetdata wordt nooit als geldige balanswaarde gebruikt. |
| **Post-condities** | De motoren worden niet meer aangestuurd met onbetrouwbare regeldata. |
| **Uitzonderingen** | • **De communicatielink naar de CYD ESP32 werkt niet:** de motoruitgangen blijven veilig en de fouttoestand blijft actief.<br>• **Meerdere fouten ontstaan tegelijk:** de foutreactie met het grootste veiligheidsrisico heeft voorrang. |

## CYD ESP32

| **[UC-CYD-01] Tonen** | |
| :--- | :--- |
| **Actor** | ESP32-S3 via ESP-NOW, gebruiker |
| **Beschrijving** | De CYD ESP32 ontvangt geldige balansgegevens en vertaalt de balansafwijking en bewegingsrichting naar expressieve ogen en een gezicht. |
| **Pre-condities** | De CYD-voeding is ingeschakeld met de eigen schakelaar. Het scherm en de ESP-NOW-ontvanger zijn geïnitialiseerd. |
| **Scenario** | 1. Een ESP-NOW-bericht wordt ontvangen.<br>2. Het formaat, de bron en de geldigheid van het bericht worden gecontroleerd.<br>3. De balansafwijking en bewegingsrichting worden gelezen.<br>4. Een gezichtsexpressie wordt gekozen die bij de robotbeweging past.<br>5. De positie, richting of vorm van de ogen wordt aangepast.<br>6. Het gezicht wordt op het scherm getekend.<br>7. De nieuwe balansgegevens worden doorgegeven aan UC-CYD-02.<br>8. De weergave wordt bij nieuwe geldige berichten bijgewerkt. |
| **Invariant** | De gezichtsexpressie is gebaseerd op het laatst ontvangen geldige bericht. |
| **Post-condities** | Het scherm toont een gezicht dat de laatst bekende bewegingstoestand van de robot weergeeft. |
| **Uitzonderingen** | • **Het bericht is ongeldig of verouderd:** het bericht wordt genegeerd en de vorige geldige toestand blijft behouden totdat de communicatietimeout verstrijkt.<br>• **De balanswaarde valt buiten het expressiebereik:** de waarde wordt begrensd tot de beschikbare gezichtstoestanden.<br>• **Er is geen geldig bericht binnen de timeout:** een communicatieverlies-expressie wordt getoond. |

| **[UC-CYD-02] Score bijhouden** | |
| :--- | :--- |
| **Actor** | ESP32-S3 via ESP-NOW, gebruiker |
| **Beschrijving** | De CYD ESP32 verhoogt een scoreboard op basis van de absolute balansafwijking en de tijd waarin de robot buiten de balansgrens blijft. De actuele score wordt onderaan het scherm getoond. |
| **Pre-condities** | De CYD-voeding is ingeschakeld en er is een geldig balansbericht ontvangen. De score is voor de huidige voedingssessie geïnitialiseerd. |
| **Scenario** | 1. De actuele balansafwijking wordt gelezen.<br>2. Er wordt gecontroleerd of de afwijking groter is dan de ingestelde balansgrens.<br>3. De scorebijdrage wordt berekend op basis van de afwijking en de verstreken tijd.<br>4. De score wordt verhoogd wanneer de robot uit balans is.<br>5. De score wordt onderaan het scherm getoond.<br>6. De score blijft bewaard zolang de CYD ingeschakeld blijft. |
| **Invariant** | De score kan tijdens één voedingssessie alleen gelijk blijven of toenemen. |
| **Post-condities** | Het scoreboard toont de cumulatieve score van de huidige voedingssessie. |
| **Uitzonderingen** | • **Er is geen geldig balansbericht:** de score wordt niet verhoogd op basis van ontbrekende data.<br>• **De maximale score is bereikt:** de score blijft op de maximale waarde staan.<br>• **De CYD wordt uitgeschakeld en opnieuw ingeschakeld:** de score wordt opnieuw geïnitialiseerd. |

| **[UC-CYD-03] Communicatieverlies tonen** | |
| :--- | :--- |
| **Actor** | ESP-NOW-verbinding, gebruiker |
| **Beschrijving** | Wanneer de CYD ESP32 geen geldig bericht meer ontvangt, maakt het scherm zichtbaar dat de getoonde balansinformatie niet actueel is. |
| **Pre-condities** | De CYD ESP32 is geïnitialiseerd en wacht op of heeft eerder een geldig bericht ontvangen. |
| **Scenario** | 1. Er verstrijkt de ingestelde timeout zonder geldig bericht.<br>2. De laatst bekende balansinformatie wordt als niet-actueel gemarkeerd.<br>3. Een communicatieverlies- of wachtindicatie wordt getoond.<br>4. Het scoreboard wordt niet verhoogd zolang geen geldige nieuwe balansinformatie beschikbaar is.<br>5. Bij een nieuw geldig bericht wordt UC-CYD-01 hervat. |
| **Invariant** | Oude balansdata wordt nooit zonder waarschuwing als actuele data getoond. |
| **Post-condities** | De gebruiker kan onderscheid maken tussen actuele robotdata en een verbroken verbinding. |
| **Uitzonderingen** | • **De verbinding wordt hersteld:** de nieuwe geldige balansgegevens worden gebruikt voor de gezichtsexpressie en score.<br>• **De verbinding blijft verbroken:** de wacht- of communicatieverlies-indicatie blijft zichtbaar. |

## Relaties tussen de usecases

- UC-S3-01 `include` UC-S3-02.
- UC-S3-02 `include` UC-S3-03.
- UC-S3-04 kan optreden als uitzondering tijdens UC-S3-01 en UC-S3-02.
- UC-CYD-01 `include` UC-CYD-02.
- UC-CYD-03 `extend` UC-CYD-01 wanneer de communicatietimeout verstrijkt.

## Nog vast te leggen ontwerpwaarden

- De MPU6050-balansas en de tekenconventie van de balanswaarde.
- De PID-parameters en de maximale motoruitgang.
- De regelperiode en ESP-NOW-verzendfrequentie.
- De communicatietimeout.
- De scorefunctie, balansgrens, maximumscore en resetregel.
- De koppeling tussen balanswaarde en gezichtsexpressie.
- De veilige motorreactie wanneer de robot buiten het regelbare kantelbereik valt.
- De laad- en beveiligingsarchitectuur van de in serie gebruikte LiPo-cellen.