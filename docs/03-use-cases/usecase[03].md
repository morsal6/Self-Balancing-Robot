| **[UC03] Automatisch balanceren** | |
| :--- | :--- |
| **Doel** | De robot met behulp van de MPU6050, de PID-regelaar en de motoren rechtop houden. |
| **Beschrijving** | De ESP32-S3 gebruikt meetwaarden van de MPU6050 en een PID-regelaar om de motoren aan te sturen en de robot in balans te houden. |
| **Pre-condities** | De ESP32-S3 en motorvoeding zijn ingeschakeld. De MPU6050 is geïnitialiseerd en levert geldige meetwaarden. |
| **Scenario** | 1. De ESP32-S3 leest de MPU6050 uit.<br>2. De ESP32-S3 bepaalt de gemeten hellingshoek.<br>3. De PID-regelaar vergelijkt de gemeten hoek met de gewenste balanshoek.<br>4. De PID-regelaar berekent de benodigde motorsturing.<br>5. De ESP32-S3 stuurt de motoren aan via de H-brug.<br>6. De ESP32-S3 herhaalt deze stappen zolang de balansregeling actief is. |
| **Post-condities** | De motoren worden aangestuurd om de robot rechtop te houden. |
| **Uitzonderingen** | • **Ongeldige of ontbrekende sensordata:** De ESP32-S3 stopt de motorsturing. |