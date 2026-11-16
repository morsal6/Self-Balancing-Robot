# Object model ESP32-S3 Super Mini

| Objectnaam | Stereotype | Beschrijving |
| :--- | :--- | :--- |
| InitControl | control | "Dirigent" van use case UC-S3-01 Initialiseren (zie use case beschrijving). Maakt na het inschakelen de sensor, de motoruitgangen, ESP-NOW en de balansregeling gereed, en start daarna BalansControl. |
| BalansControl | control | "Dirigent" van use case UC-S3-02 Balanceren (zie use case beschrijving). Verwerkt elke meting tot een correctie en stuurt de motoren aan om de robot rechtop te houden. |
| BalansverliesControl | control | "Dirigent" van use case UC-S3-03 Balansverlies afhandelen (zie use case beschrijving). Stopt de motoren na een val, verhoogt de valteller en hervat het balanceren zodra de robot weer rechtop staat. |
| VerstuurControl | control | "Dirigent" van use case UC-S3-04 Versturen (zie use case beschrijving). Verzamelt periodiek de balansgegevens, status en valteller en verstuurt die naar de CYD. |
| FoutControl | control | "Dirigent" van use case UC-S3-05 Fouttoestand afhandelen (zie use case beschrijving). Zet bij een sensor-, initialisatie- of regelingsfout de motoren veilig op nul en de status op FOUT. |
| Mpu6050 | boundary | Stuurt de MPU6050-hardware aan en meldt nieuwe accelerometer- en gyroscoopmetingen. |
| MotorDriver | boundary | Stuurt via de ZK5AD-h-brug de linker- en rechter-TT130-motor aan (richting en snelheid). |
| EspNowZender | boundary | Proxy naar de CYD ESP32: configureert de ESP-NOW-verbinding en verstuurt berichten. |
| RobotStatus | entity | Bevat de actuele status van de robot: KALIBREREN, BALANCEREN, BALANSVERLIES of FOUT. |
| Sessie | entity | Bevat het sessie-ID en de valteller. Deze data wordt als pakket naar de CYD verstuurd. |
| BalansData | entity | Bevat de actuele balanswaarde, de bewegingsrichting en of deze gegevens geldig zijn. |
| BalansFilter | — | Combineert accelerometer- en gyroscoopmetingen tot een balanswaarde en bewaart het nulpunt. |
| PidRegelaar | — | Berekent met de PID-parameters een begrensd correctiecommando uit de afwijking ten opzichte van het nulpunt. |