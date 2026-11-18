# Object beschrijving: object model ESP32-S3 Super Mini

| Objectnaam | Stereotype | Beschrijving |
| :--- | :--- | :--- |
| RobotControl | control | "Dirigent" van use case UC-S3-00 Robot besturen (zie use case beschrijving). Voert na elkaar Initialiseren, Balanceren en Balansverlies afhandelen uit, voert bij een fout Fouttoestand afhandelen uit, en geeft iedere nieuwe fase door. Ontvangt de metingen van de Mpu6050 en geeft ze door aan de control die op dat moment wordt uitgevoerd. |
| InitControl | control | "Dirigent" van use case UC-S3-01 Initialiseren (zie use case beschrijving). Wordt uitgevoerd door RobotControl. Maakt sensor, motoruitgangen en ESP-NOW gereed en bepaalt het nulpunt zodra de robot lang genoeg stilstaat. |
| BalansControl | control | "Dirigent" van use case UC-S3-02 Balanceren (zie use case beschrijving). Wordt uitgevoerd door RobotControl. Verwerkt iedere meting tot een correctie voor de motoren en stelt een balansverlies vast. |
| BalansverliesControl | control | "Dirigent" van use case UC-S3-03 Balansverlies afhandelen (zie use case beschrijving). Wordt uitgevoerd door RobotControl. Stopt de motoren, verhoogt de valteller en wacht tot de robot weer rechtop en stil staat. |
| FoutControl | control | "Dirigent" van use case UC-S3-05 Fouttoestand afhandelen (zie use case beschrijving). Wordt uitgevoerd door RobotControl. Zet de motoren veilig op nul en markeert de balansinformatie als ongeldig. |
| VerstuurControl | control | "Dirigent" van use case UC-S3-04 Versturen (zie use case beschrijving). Draait parallel aan RobotControl en verstuurt periodiek, of bij een nieuwe fase of valteller, een bericht naar de CYD. |
| Mpu6050 | boundary | Stuurt de MPU6050-hardware aan en meldt iedere regelperiode een nieuwe accelerometer- en gyroscoopmeting. |
| MotorDriver | boundary | Stuurt via de ZK5AD-h-brug de linker- en rechter-TT130-motor aan (richting en snelheid). |
| EspNowZender | boundary | Proxy naar de CYD ESP32: configureert de ESP-NOW-verbinding en verstuurt berichten. |
| RobotFase | entity | Bevat de fase waarin de robot zich bevindt: KALIBREREN, BALANCEREN, BALANSVERLIES of FOUT. |
| Sessie | entity | Bevat het sessie-ID en de valteller. Deze data wordt als pakket naar de CYD verstuurd. |
| BalansData | entity | Bevat de actuele balanswaarde, de bewegingsrichting en of deze gegevens geldig zijn. |
| BalansFilter | — | Combineert accelerometer- en gyroscoopmetingen tot een balanswaarde en bewaart het nulpunt. |
| PidRegelaar | — | Berekent met de PID-parameters een begrensd correctiecommando uit de afwijking ten opzichte van het nulpunt. |