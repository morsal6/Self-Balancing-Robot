| **[UC04] Omvallen afhandelen** | |
| :--- | :--- |
| **Actor** | Gebruiker (indirect, via UC02) |
| **Doel** | Voorkomen dat de wielen blijven draaien als de robot omvalt, en de robot klaarzetten voor een nieuwe poging. |
| **Beschrijving** | Zodra de hellingshoek buiten de veilige grens komt, stoppen de wielen. Zodra de gebruiker de robot weer rechtop houdt, start het balanceren automatisch opnieuw. |
| **Pre-condities** | Balanceren (UC02) is actief. |
| **Scenario** | 1. Zodra de hellingshoek buiten de veilige grens komt, stoppen de wielen.<br>2. De status wordt OMGEVALLEN.<br>3. Het omvallen wordt verwerkt in de statistieken (zie UC06).<br>4. Er wordt gewacht tot de gebruiker de robot weer rechtop houdt: de hellingshoek blijft een korte, vaste tijd binnen de startgrens.<br>5. Het balanceren (UC02) start automatisch opnieuw. |
| **Invariant** | Totdat het balanceren opnieuw start (stap 5), blijven de wielen stilstaan, ook als de hellingshoek intussen weer binnen de veilige grens komt. |
| **Post-condities** | Het balanceren (UC02) is opnieuw actief, met een huidige balanstijd die weer bij nul begint. |
| **Uitzonderingen** | Geen. Omvallen is in deze use case het normale scenario. |