### Objectmodellen

De objectmodellen zijn opgesteld met de Design Like a Robot-methode: per usecase één control-object, daarna zijn de usecasebeschrijvingen zin voor zin doorlopen. Voor elk "ding" in een zin is een object gemaakt en voor elke handeling een bericht langs een pijl, van het sprekende naar het luisterende object.

Een objectmodel beschrijft de software van één computer. Daarom zijn er twee modellen, één voor de ESP32-S3 Super Mini en één voor de CYD ESP32. De ESP-NOW-verbinding loopt via de proxy-objecten `CydProxy` en `RobotProxy`. Timers zijn net als andere waitables niet als object opgenomen.

![Objectmodel ESP32-S3](/media/images/objectmodel-esp32s3.png)

![Objectmodel CYD](/media/images/objectmodel-cyd.png)

Ontwerpkeuzes die niet direct uit de usecases volgen:

- `Mpu6050` levert direct een gefilterde balanswaarde ten opzichte van het nulpunt (UC-S3-02, stap 1, 2 en 4). Het filteren gebeurt dus in de sensor zelf, zodat de drie controls die de balanswaarde gebruiken niet elk zelf hoeven te filteren.
- De PID-berekening (UC-S3-02, stap 5 tot 7) zit in `BalansControl`, omdat alleen die hem gebruikt.
- `BalansControl` is het enige object dat tijdens het balanceren motorcommando's geeft. Daarom zet `BalansControl` de motoren zelf stil bij een balansverlies, voordat hij `balansVerloren` meldt (UC-S3-02, uitzondering, en UC-S3-03, stap 3). Bij elke `start` initialiseert hij zijn PID-regeling opnieuw en zet hij de status op `BALANCEREN` (UC-S3-01, stap 8 en 9, en UC-S3-03, stap 4, 9 en 10).
- `FoutControl` stuurt alleen `stop` naar `BalansControl`. Na een `stop` negeert `BalansControl` elke volgende `start` tot een herstart. Daardoor kunnen `InitialiseerControl` en `BalansverliesControl` de robot niet meer laten rijden. Ook kan de valteller niet meer veranderen, want die wordt alleen verhoogd na `balansVerloren`. Zo blijft de fouttoestand actief tot een herstart (UC-S3-05, stap 6).
- `Mpu6050` en `Hbrug` melden zelf hun fouten, volgens de regel dat boundary-objecten informatie sturen in plaats van dat controls pollen.

#### Lijst met Objecten

**ESP32-S3 Super Mini**

| Object Naam | Stereotype | Beschrijving |
| :--- | :--- | :--- |
| InitialiseerControl | Control | "Dirigent" van de usecase *Initialiseren* (zie [UC-S3-01](https://github.com/morsal6/Self-Balancing-Robot/tree/main/docs/03-use-cases)). |
| BalansControl | Control | "Dirigent" van de usecase *Balanceren* (zie UC-S3-02). |
| BalansverliesControl | Control | "Dirigent" van de usecase *Balansverlies afhandelen* (zie UC-S3-03). |
| VerstuurControl | Control | "Dirigent" van de usecase *Versturen* (zie UC-S3-04). |
| FoutControl | Control | "Dirigent" van de usecase *Fouttoestand afhandelen* (zie UC-S3-05). |
| Mpu6050 | Boundary | Leest periodiek de versnelling en hoeksnelheid uit een MPU6050, controleert of de metingen geldig zijn en combineert ze tot een gefilterde balanswaarde ten opzichte van een instelbaar nulpunt. |
| Hbrug | Boundary | Stuurt via een ZK5AD-h-brug de linker- en rechtermotor aan met een richting en snelheid per motor, of zet beide motoren stil. |
| Balansgegevens | Entity | Bevat de actuele balanswaarde, bewegingsrichting, status, valteller, het sessie-ID en of de balansinformatie geldig is. |
| CydProxy | Boundary | Representeert de CYD ESP32: verstuurt balansgegevens met een volgnummer via ESP-NOW. |

**CYD ESP32**

| Object Naam | Stereotype | Beschrijving |
| :--- | :--- | :--- |
| ToonControl | Control | "Dirigent" van de usecase *Tonen* (zie UC-CYD-01). |
| ScoreControl | Control | "Dirigent" van de usecase *Score bijhouden* (zie UC-CYD-02). |
| CommunicatieverliesControl | Control | "Dirigent" van de usecase *Communicatieverlies tonen* (zie UC-CYD-03). |
| RobotProxy | Boundary | Representeert de ESP32-S3: ontvangt ESP-NOW-berichten en laat alleen berichten met een geldig formaat, een geldige bron en een nieuw volgnummer door. |
| Gezicht | – | Zet een expressie, intensiteit en kijkrichting om in de vorm en positie van twee ogen. |
| Display | Boundary | Stuurt het scherm van de CYD aan: tekent de ogen in het bovenste deel en de score in het onderste deel. |

#### Herleiding van usecasestappen naar berichten

| Usecase | Stap | Bericht(en) |
| :--- | :--- | :--- |
| UC-S3-01 | 1–2 | `InitialiseerControl → Balansgegevens: setStatus(status), setSessieId(sessieId), resetValteller` |
| | 3 | `InitialiseerControl → Hbrug: stopMotoren` |
| | 4 | `InitialiseerControl → Mpu6050: start` |
| | 5 | `InitialiseerControl → CydProxy: configureer` |
| | 6–7 | `Mpu6050 → InitialiseerControl: nieuweBalanswaarde(...)`, `InitialiseerControl → Mpu6050: setNulpunt(nulpunt)` |
| | 8–9 | `InitialiseerControl → BalansControl: start` (BalansControl initialiseert de PID-regeling) |
| UC-S3-02 | 1–4 | `Mpu6050 → BalansControl: nieuweBalanswaarde(balanswaarde, hoeksnelheid)` |
| | 5–7 | binnen `BalansControl` (PID-berekening) |
| | 8–9 | `BalansControl → Hbrug: stuurMotoren(linkerCommando, rechterCommando)` |
| | 10 | `BalansControl → Balansgegevens: setBalans(balanswaarde, bewegingsrichting)` |
| UC-S3-03 | 1–4 | `BalansControl → Hbrug: stopMotoren`, `BalansControl → BalansverliesControl: balansVerloren`, `BalansverliesControl → Balansgegevens: setStatus(status)` |
| | 5 | `BalansverliesControl → Balansgegevens: verhoogValteller` |
| | 8 | `Mpu6050 → BalansverliesControl: nieuweBalanswaarde(...)` |
| | 9–10 | `BalansverliesControl → BalansControl: start` |
| UC-S3-04 | 1–4 | `VerstuurControl → Balansgegevens: getBalansgegevens`, `VerstuurControl → CydProxy: verstuur(balansgegevens)` |
| UC-S3-05 | 1 | `Mpu6050 → FoutControl: sensorFout(foutcode)`, `Hbrug → FoutControl: motoruitgangFout(foutcode)`, `BalansControl → FoutControl: regelFout(foutcode)` |
| | 2–4 | `FoutControl → Balansgegevens: markeerOngeldig, setStatus(status)`, `FoutControl → Hbrug: stopMotoren`, `FoutControl → BalansControl: stop` |
| UC-CYD-01 | 1–3 | `RobotProxy → ToonControl: balansgegevensOntvangen(balansgegevens)` |
| | 4–6 | `ToonControl → Gezicht: toonExpressie(expressie, intensiteit, kijkrichting)`, `Gezicht → Display: tekenOgen(ogen)` |
| | 7 | `ToonControl → ScoreControl: valtellerOntvangen(valteller, sessieId)` |
| UC-CYD-02 | 7 | `ScoreControl → Display: tekenScore(score)` |
| UC-CYD-03 | 1 | `RobotProxy → CommunicatieverliesControl: berichtOntvangen` (start de timeout opnieuw) |
| | 2–3 | `CommunicatieverliesControl → Gezicht: toonExpressie(expressie, intensiteit, kijkrichting)` |
