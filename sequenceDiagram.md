

Images créées sur: https://sequencediagram.org/#

```
title Diagramme de séquence

actor User
participant GUI
participant Algo évolutif
participant Algo recherche
participant Stats
participant Logs
User->GUI: L'utilisateur rentre les données souhaitées avec un set de données (ou snapshot) à analyser
GUI->Algo évolutif: Passe la main à la partie calcul

loop #2f2e7b #white un certain nombre d'itérations défini par User
Algo évolutif->Algo recherche: Fournit un pool de mots généré aléatoirement à partir d'un autre pool de mots (croisement)
Algo recherche->Stats: Fournit des pairs mots-caractéristiques
Stats->Logs: Écrit les résultats dans des logs
Stats->GUI: Affiche les resultats (comparaison entre le pool généré avec le set de données de base)
GUI->User: Affiche le résultat sous forme de graphique
end
User->GUI: Peut enregistrer les données
note right of GUI: Enregistre l'ensemble des résultats formatés dans un fichier
```
