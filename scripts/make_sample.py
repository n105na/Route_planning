import csv

INPUT_NODES = "data/processed/nodes.csv"
INPUT_EDGES = "data/processed/edges.csv"

OUTPUT_NODES = "data/sample/nodes_small.csv"
OUTPUT_EDGES = "data/sample/edges_small.csv"

MAX_EDGES = 5000  # adjust if needed


# 🔹 Step 1: read first N edges (already valid graph)
edges_raw = []

with open(INPUT_EDGES, newline='', encoding="utf-8") as f:
    reader = csv.DictReader(f)
    for i, row in enumerate(reader):
        if i >= MAX_EDGES:
            break
        edges_raw.append(row)


# 🔹 Step 2: collect used nodes
used_nodes = set()

for e in edges_raw:
    used_nodes.add(int(e["src"]))
    used_nodes.add(int(e["dst"]))


# 🔹 Step 3: create NEW mapping (important!)
old_to_new = {}
new_nodes = []

with open(INPUT_NODES, newline='', encoding="utf-8") as f:
    reader = csv.DictReader(f)
    for row in reader:
        old_id = int(row["node_id"])

        if old_id in used_nodes:
            new_id = len(new_nodes)
            old_to_new[old_id] = new_id

            new_nodes.append({
                "node_id": new_id,
                "osm_id": row["osm_id"],
                "lat": row["lat"],
                "lon": row["lon"]
            })


# 🔹 Step 4: rebuild edges with NEW IDs
new_edges = []

for e in edges_raw:
    old_src = int(e["src"])
    old_dst = int(e["dst"])

    if old_src in old_to_new and old_dst in old_to_new:
        new_edges.append({
            "src": old_to_new[old_src],
            "dst": old_to_new[old_dst],
            "weight": e["weight"]
        })


# 🔹 Step 5: sort edges (CRUCIAL for CSR)
new_edges.sort(key=lambda x: int(x["src"]))


# 🔹 Step 6: save nodes
with open(OUTPUT_NODES, "w", newline='', encoding="utf-8") as f:
    writer = csv.DictWriter(f, fieldnames=["node_id", "osm_id", "lat", "lon"])
    writer.writeheader()
    writer.writerows(new_nodes)


# 🔹 Step 7: save edges
with open(OUTPUT_EDGES, "w", newline='', encoding="utf-8") as f:
    writer = csv.DictWriter(f, fieldnames=["src", "dst", "weight"])
    writer.writeheader()
    writer.writerows(new_edges)


print(f"Sample created: {len(new_nodes)} nodes, {len(new_edges)} edges")