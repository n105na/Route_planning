
---

## Reproduction complète (step-by-step)

Cette section décrit précisément comment reconstruire le projet depuis zéro, depuis les données brutes jusqu’aux figures finales.

---
### Étape 0 — Installer les dépendances
Créer un environnement virtuel (recommandé) :
```
python3 -m venv venv
source venv/bin/activate
```

Installer les dépendances Python :
```
pip install -r requirements.txt
```

Les principales bibliothèques utilisées sont :

* `numpy` → traitement des données
* `matplotlib` → génération des figures
* `osmium` → extraction des données OpenStreetMap
### Étape 1 — Télécharger les données OpenStreetMap

Télécharger l’extrait Île-de-France :

```bash
wget https://download.geofabrik.de/europe/france/ile-de-france-latest.osm.pbf -O data/raw/ile-de-france.osm.pbf
````

Ce fichier contient :

* l’ensemble des routes, bâtiments, points d’intérêt, etc.
* un volume de données très important → nécessite filtrage

---

### Étape 2 — Extraire le graphe routier

Exécuter :

```bash
python scripts/extract_roads_osmium.py
```

Ce script :

* parcourt le fichier `.osm.pbf` avec **osmium**
* filtre uniquement les routes (`highway`)
* récupère les coordonnées géographiques des nœuds
* construit un graphe orienté :

  * respecte les routes à sens unique (`oneway`)
* calcule les poids avec la formule de Haversine :

```
d = 2R * \arcsin( sqrt(...) )
```

* génère deux fichiers :

```
data/processed/nodes.csv
data/processed/edges.csv
```

  Les arêtes sont triées par source pour faciliter la construction CSR 

---

### Étape 3 — Générer un sous-graphe (100k nœuds)

Exécuter :

```bash
python scripts/make_sample.py
```

Ce script :

* charge les arêtes sous forme de liste d’adjacence
* effectue un **BFS** à partir d’un nœud initial
* construit un sous-graphe **connecté**
* limite à `MAX_NODES = 100000`

Ensuite :

* sélection des nœuds atteints
* **remapping des IDs** vers `[0, n-1]`
* reconstruction des arêtes cohérentes
* tri des arêtes pour CSR

Sortie :

```
data/sample/nodes_small.csv
data/sample/edges_small.csv
```

  garantit un graphe exploitable par les algorithmes 

---

### Étape 4 — Compiler le benchmark

```bash
gcc -o bench src/benchmark/benchmark.c \
src/graph/graph.c src/heap/heap.c \
src/algorithms/dijkstra.c src/algorithms/astar.c \
src/algorithms/alt.c src/algorithms/ch.c -lm
```

Compilation des modules :

* structure CSR
* heap (file de priorité)
* algorithmes

---

### Étape 5 — Exécuter les tests

```bash
./bench
```

Le benchmark :
* Le fichier CSV est généré automatiquement pendant le benchmark et contient les résultats par requête.
* génère 500 requêtes aléatoires `(s, t)`
* utilise une graine fixe (`srand(42)`) → reproductibilité
* classe les requêtes :

  * courte / moyenne / longue distance
* exécute :

  * Dijkstra
  * A*
  * ALT
  * CH

Mesures effectuées :

* temps par requête
* moyenne, p50, p95
* débit (requêtes/seconde)
* coût mémoire
* temps de prétraitement

Validation :

* comparaison avec Dijkstra
* détection des erreurs (`Mismatch`)

Sortie :

```
results/csv/results.csv
```

---

### Étape 6 — Générer les figures

```bash
python scripts/generate_plots.py
```
Les valeurs correspondant à des distances infinies (graphes non connectés) sont ignorées pour garantir des statistiques fiables. 
Ce script :

1. charge le fichier CSV
2. filtre les valeurs invalides :

   * distances infinies
   * valeurs extrêmes (DBL_MAX)
3. supprime les outliers (temps > 1000 ms)
4. calcule :

   * moyenne
   * p50
   * p95
5. génère plusieurs figures :

* histogramme des temps
* temps moyen
* p50
* p95
* version log-scale

  permet d’analyser la distribution réelle des performances 

Sortie :

```
results/figures/
```

---

## Pipeline global

```text
OSM (.pbf)
   ↓
extract_roads_osmium.py
   ↓
nodes.csv / edges.csv
   ↓
make_sample.py (BFS, 100k)
   ↓
nodes_small.csv / edges_small.csv
   ↓
benchmark (./bench)
   ↓
results.csv
   ↓
generate_plots.py
   ↓
figures
```

---

## Points importants

* Le graphe complet est trop volumineux → échantillonnage nécessaire
* Le remapping des IDs est essentiel pour CSR
* Les résultats sont reproductibles (seed fixe)
* Les valeurs infinies sont filtrées pour éviter de fausser les statistiques

