# Route Planning — M1 Structures de données avancées

Projet : répondre rapidement à des requêtes de plus court chemin sur un réseau routier réel.

## Objectif

Le projet consiste à comparer plusieurs méthodes de plus court chemin sur un graphe routier extrait depuis OpenStreetMap :

- Dijkstra
- A*
- ALT
- CH

L'objectif n'est pas seulement d'obtenir un algorithme correct, mais aussi d'étudier :

- le temps par requête,
- le coût mémoire,
- le coût du prétraitement,
- les compromis entre simplicité, rapidité et stockage.

---

## Source des données

Nous utilisons des données réelles OpenStreetMap sur la zone **Île-de-France**.

Lien direct vers l’extrait utilisé :

https://download.geofabrik.de/europe/france/ile-de-france-latest.osm.pbf

Source de téléchargement :

https://download.geofabrik.de/europe/france/ile-de-france.html

---

## Pipeline des données

Le pipeline est le suivant :

1. téléchargement d’un extrait OpenStreetMap brut (`.osm.pbf`)
2. extraction des routes pertinentes
3. conversion en graphe orienté pondéré
4. export en fichiers CSV
5. chargement en C et conversion vers une représentation compacte de type CSR

---

## Modélisation du graphe

Le réseau routier est modélisé comme un graphe orienté pondéré :

- **sommet / node** = point du réseau routier
- **arête / edge** = segment de route entre deux points consécutifs
- **poids / weight** = distance géographique approximative en mètres

### Important

Dans cette version, le poids n'est pas lu directement comme un coût prêt à l'emploi depuis OpenStreetMap.

Le poids est **calculé** à partir des coordonnées géographiques des deux extrémités d'une arête, avec une distance de type **haversine**, afin d'obtenir une estimation de la distance en mètres.

Ainsi, le graphe actuel est un **graphe de distance**, et non encore un graphe de temps de trajet.

---

## Données du dépôt

### `data/processed/`
Contient les données réelles déjà converties en CSV pour permettre un test rapide sans devoir retraiter tout l’extrait OSM.

Fichiers attendus :

- `nodes.csv`
- `edges.csv`

### `data/sample/`
Contient un petit jeu de données réduit pour les tests très rapides et le débogage.

### `data/raw/`
Contient l’extrait brut OpenStreetMap téléchargé localement.

Ce dossier est **ignoré par Git** car les fichiers sont volumineux, mais la source exacte est documentée dans ce README.

---

## Format des fichiers

### `nodes.csv`

Colonnes :

- `node_id` : identifiant interne utilisé par le projet
- `osm_id` : identifiant original OpenStreetMap
- `lat` : latitude
- `lon` : longitude

Exemple :

```csv
node_id,osm_id,lat,lon
0,123456,48.8566,2.3522
1,123457,48.8568,2.3525