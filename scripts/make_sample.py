import csv
from collections import deque

INPUT_NODES = "data/processed/nodes.csv"
INPUT_EDGES = "data/processed/edges.csv"

OUTPUT_NODES = "data/sample/nodes_small.csv"
OUTPUT_EDGES = "data/sample/edges_small.csv"

MAX_NODES = 100000


# Chargement des arêtes en liste d'adjacence
adj = {}

with open(INPUT_EDGES, newline='', encoding="utf-8") as f:
    reader = csv.DictReader(f)
    for row in reader:
        u = int(row["src"])
        v = int(row["dst"])
        w = float(row["weight"])

        if u not in adj:
            adj[u] = []

        adj[u].append((v, w))


# BFS pour garantir la connexité du sous-graphe
visited = set()
queue = deque([0])   # start from node 0

edges = []
nodes_set = set([0])

while queue and len(nodes_set) < MAX_NODES:
    u = queue.popleft()

    if u not in adj:
        continue

    for v, w in adj[u]:
        edges.append((u, v, w))

        if v not in visited:
            visited.add(v)
            nodes_set.add(v)
            queue.append(v)

        if len(nodes_set) >= MAX_NODES:
            break


# Construction de la liste des nœuds sélectionnés
nodes = []

with open(INPUT_NODES, newline='', encoding="utf-8") as f:
    reader = csv.DictReader(f)
    for row in reader:
        nid = int(row["node_id"])
        if nid in nodes_set:
            nodes.append(row)


# Réindexation des identifiants
old_to_new = {}
new_nodes = []

for i, row in enumerate(nodes):
    old_id = int(row["node_id"])
    old_to_new[old_id] = i

    new_nodes.append({
        "node_id": i,
        "osm_id": row["osm_id"],
        "lat": row["lat"],
        "lon": row["lon"]
    })


# Mise à jour des arêtes avec les nouveaux IDs
new_edges = []

for u, v, w in edges:
    if u in old_to_new and v in old_to_new:
        new_edges.append({
            "src": old_to_new[u],
            "dst": old_to_new[v],
            "weight": w
        })


# Tri final pour le format CSR
new_edges.sort(key=lambda x: x["src"])


# Exportation des fichiers CSV
with open(OUTPUT_NODES, "w", newline='', encoding="utf-8") as f:
    writer = csv.DictWriter(f, fieldnames=["node_id", "osm_id", "lat", "lon"])
    writer.writeheader()
    writer.writerows(new_nodes)

with open(OUTPUT_EDGES, "w", newline='', encoding="utf-8") as f:
    writer = csv.DictWriter(f, fieldnames=["src", "dst", "weight"])
    writer.writeheader()
    writer.writerows(new_edges)

print(f"Connected sample: {len(new_nodes)} nodes, {len(new_edges)} edges")
