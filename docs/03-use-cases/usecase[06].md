| **[UC06] Omvallen detecteren en veilig stoppen** | |
| :--- | :--- |
| **Doel** | Voorkomen dat de motoren blijven aandrijven wanneer de robot omvalt of te ver overhelt. |
| **Beschrijving** | De ESP32-S3 stopt de motorsturing wanneer de robot buiten de ingestelde veilige hellingshoek komt. |
| **Pre-condities** | De ESP32-S3 is ingeschakeld en ontvangt meetwaarden van de MPU6050. |
| **Scenario** | 1. De ESP32-S3 bepaalt de actuele hellingshoek.<br>2. De ESP32-S3 vergelijkt de hellingshoek met de ingestelde veilige grens.<br>3. Als de grens wordt overschreden, schakelt de ESP32-S3 de motorsturing uit.<br>4. De ESP32-S3 stuurt zo mogelijk een foutstatus naar de CYD.<br>5. De gebruiker zet de robot weer rechtop voordat de balansregeling opnieuw kan starten. |
| **Post-condities** | De motoren worden niet aangestuurd zolang de robot buiten de ingestelde veilige hellingshoek staat. |
| **Uitzonderingen** | • **Veilige hellingshoek aangehouden:** De grens wordt niet overschreden en de motorsturing wordt niet vanwege omvallen uitgeschakeld. |