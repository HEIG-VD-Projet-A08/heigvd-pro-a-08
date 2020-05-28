# Outil pour la recherche de séquences génomiques

## Description

Outil pour traiter des informations génomiques, issues de bases de données publiques, dans un but prédictif (par exemple prédire la localisation d'une protéine dans une cellule). L’outil sera basé sur l’utilisation d’un algorithme évolutionniste, une technique d'apprentissage automatique, pour la recherche automatique des séquences

Une bonne partie du comportement des êtres vivantes est définie par son génome (ADN) et par les protéines qui en découlent. L’analyse des informations contenues dans le génome d’un organisme facilite la compréhension de nombreux phénomènes. Ces informations génomiques sont représentées en forme de (longues) séquences de caractères. Que ce soit de l’ADN, dont les séquences sont composées par un alphabet de 4 lettres *(A,C,G,T)*, ou des protéines représentées par un alphabet de 20 lettres.

Une forme de traiter et de décrire de telles séquences consiste à trouver des sous-séquences particulières responsables du comportement souhaité (la localisation dans l'exemple). Néanmoins, l'identification des sous-séquences adéquates pour une analyse particulier étant très complexe, on propose d’utiliser une technique d’apprentissage automatique à ce fin: un algorithme évolutionniste simple.

L’outil développé devra permettre à un utilisateur d’appliquer la méthode décrite ci-dessus à une collection de séquences fournie ainsi qu'à visualiser des aspects clé du processus de recherche (statistiques simples).

This software was developed as semester project (PRO) at HEIG-VD,
academic year 2019/20.

Development team:

| Name                                 | Email                        | Github   |
|--------------------------------------|------------------------------|----------|
| Jérôme Arn                           | jerome.arn@heig-vd.ch        | hans-arn |
| Chris Barros Henriques (project lead) | chris.barroshenriques@heig-vd.ch | kurisukun |
| Jean-Luc Blanc       | jean-luc.blanc2@heig-vd.ch | JL-Heig |
| Tiffany Bonzon      | tiffany.bonzon@heig-vd.ch | tiffanybonzon |
| Gaëtan Daubresse | gaetan.daubresse@heig-vd.ch | Gaetan2907 |
| Quentin Saucy (deputy project lead) | quentin.saucy@heig-vd.ch | qsaucy |



## Dependencies



### Serveur

Le serveur dépend de Python3 pour pouvoir fonctionner.  Python2 étant déprecié, il n’y a plus aucune raison d’utiliser cette version. Toutes les librairies nécessaires sont spécifiées dans les fichiers source, mais en voici une liste non-exhaustive des plus importantes:

- xml.dom.minidom
- socket
- deap
- server
- threading



Afin d’utiliser le clone de ce dépôt comme serveur, il est important de noter qu’il faut télécharger l’exécutable de BLASTp sur el site du NCBI en suivant le tout premier lien et indiquant le téléchargement de BLAST+:

https://blast.ncbi.nlm.nih.gov/Blast.cgi?CMD=Web&PAGE_TYPE=BlastDocs&DOC_TYPE=Download

À noter que seul l’exécutable *blastp* est important et doit être placé à la racine du dossier *ServeurPython*.



### GUI

Ce repo permet de compiler le code de la gui en utilisant l'environnement de développement Qt Creator, version 4.12, qui utilise la version 5.14 de Qt. L'exécutable se trouvera dans les dossiers de build qui se composeront lors de la configuration du projet sur Qt Creator en ouvrant le fichier GUI.pro qui sert de fichier de projet pour la GUI. A noter qu'il est important d'avoir une version de Qt au minimum à 5.12 afin de garantir le fonctionnement du widget "QcustomPlot" qui permet de gérer tous l'espace graphique de la GUI.



## Build and install 

Il y a deux choix possibles:

- Cloner ce dépôt et à ce moment il faut reconfigurer certaines globales telles que l’adresse IP du serveur dans le fichier *server.py* afin que cela corresponde à votre configuration. 
- Soit suivre le document d’installation fourni [ici](https://docs.google.com/document/d/1seFQkZeonm4pClltEjApgphUv92bZIkGLG9tfxqKCng/edit) et utilisant deux VMs pour que l’application soit portable pour tous les OS. 



## Run



### Serveur

Pour démarrer le serveur, on se place à la racine du dossier *ServeurPython* et on lance la commande:

`python3 evolutionaryAlgorithm.py`



### GUI

- Si on clone le dépôt, il faut d’abord compiler les sources avec Qt, puis lancer l’exécutable produit par Qt
- Si on utilise la VM de le GUI, il suffit de cliquer sur le raccourci se trouvant sur le bureau



Des informations complémentaires sont aussi disponibles sur le [document d’installation](https://docs.google.com/document/d/1seFQkZeonm4pClltEjApgphUv92bZIkGLG9tfxqKCng/edit#).



## Use

Une vidéo d’utilisation est disponible [ici](https://www.youtube.com/watch?v=fWR4iqKPJ_M).

De plus, deux documents expliquant respectivement l’utilisation du serveur, ainsi que de la GUI sont disponibles [ici](https://docs.google.com/document/d/1XxEX0TbW0xTVOsK6n1MtslHF3tro2CSoDP8Re_BvA-A/edit) et [ici](https://docs.google.com/document/d/13-yBwZSqTa4TsCkI1R6mElRo3i1SPPG9_vb-g1qCZ0k/edit).



## Documentation



**Documentations techniques:**

- [Serveur](https://docs.google.com/document/d/1cu2fy_NKeB7CfN1gLJ-30kh-27nX6uxqZ55I2D_rS-U/edit#heading=h.z6ne0og04bp5)
- [GUI](https://docs.google.com/document/d/1n5Kse2fOryqGmdbbhAouZGntiE6GZY5HOwPQOgFq6KI/edit#heading=h.z6ne0og04bp5)
- [Protocole de transmission](https://drive.google.com/drive/u/0/folders/1-itmTpJNswW27KFAy4zYQRq1HExekpTE) 



**Organisation du code:**

Trouvable [ici](https://docs.google.com/document/d/1UPgO1qawN50WhleWUzYOhT0A8MWyMeFjGUHenDAAZOA/edit).



**Suivi de qualité:**

Trouvable [ici](https://docs.google.com/document/d/1AetQJ7J8tCzK8aHNvAt9HnDwHp69Wdf0ZDc_uYqlTHA/edit).
