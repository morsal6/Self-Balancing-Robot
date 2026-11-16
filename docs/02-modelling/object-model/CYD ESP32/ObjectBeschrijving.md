# Object model CYD ESP32

| Objectnaam | Stereotype | Beschrijving |
| :--- | :--- | :--- |
| TonenControl | control | "Dirigent" van use case UC-CYD-01 Tonen (zie use case beschrijving). Controleert ontvangen berichten, kiest de gezichtsexpressie en geeft de valteller door aan ScoreControl. |
| ScoreControl | control | "Dirigent" van use case UC-CYD-02 Score bijhouden (zie use case beschrijving). Bewaart de score, het referentie-sessie-ID en de referentie-valteller, en laat de score onderaan het scherm tonen. |
| CommVerliesControl | control | "Dirigent" van use case UC-CYD-03 Communicatieverlies tonen (zie use case beschrijving). Markeert de robotdata als verouderd en toont een communicatieverlies-expressie zolang er geen geldig bericht is. |
| EspNowOntvanger | boundary | Ontvangt via ESP-NOW de berichten van de ESP32-S3 en meldt elk ontvangen bericht. |
| Display | boundary | Stuurt het scherm van de CYD aan: het gezicht bovenin en de score onderaan. |
| RobotData | entity | Bevat de gegevens uit het laatst ontvangen geldige bericht (status, balansafwijking, bewegingsrichting en volgnummer) en of deze nog actueel zijn. |