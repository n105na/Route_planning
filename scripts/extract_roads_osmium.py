import osmium
import csv
import math

INPUT_FILE = "data/raw/ile-de-france.osm.pbf"
NODES_CSV = "data/processed/nodes.csv"
EDGES_CSV = "data/processed/edges.csv"

HIGHWAY_ALLOWED = {
    "motorway", "trunk", "primary", "secondary", "tertiary",
    "unclassified", "residential", "service",
    "motorway_link", "trunk_link", "primary_link",
    "secondary_link", "tertiary_link", "living_street"
}

def haversine_m(lat1, lon1, lat2, lon2):
    R = 6371000.0
    p1 = math.radians(lat1)
    p2 = math.radians(lat2)
    dphi = math.radians(lat2 - lat1)
    dl = math.radians(lon2 - lon1)
    a = math.sin(dphi / 2) ** 2 + math.cos(p1) * math.cos(p2) * math.sin(dl / 2) ** 2
    return 2 * R * math.asin(math.sqrt(a))

class RoadCollector(osmium.SimpleHandler):
    def __init__(self):
        super().__init__()
        self.node_coords = {}
        self.used_nodes = set()
        self.ways = []

    def node(self, n):
        if n.location.valid():
            self.node_coords[n.id] = (n.location.lat, n.location.lon)

    def way(self, w):
        highway = w.tags.get("highway")
        if highway not in HIGHWAY_ALLOWED:
            return

        refs = [nd.ref for nd in w.nodes]
        if len(refs) < 2:
            return

        oneway = w.tags.get("oneway") in {"yes", "1", "true"}
        self.ways.append((refs, oneway))

        for ref in refs:
            self.used_nodes.add(ref)

handler = RoadCollector()
handler.apply_file(INPUT_FILE, locations=True)

osm_to_internal = {}
nodes_out = []

for osm_id in sorted(handler.used_nodes):
    if osm_id not in handler.node_coords:
        continue
    internal_id = len(nodes_out)
    osm_to_internal[osm_id] = internal_id
    lat, lon = handler.node_coords[osm_id]
    nodes_out.append((internal_id, osm_id, lat, lon))

edges_out = []

for refs, oneway in handler.ways:
    filtered = [r for r in refs if r in osm_to_internal]
    if len(filtered) < 2:
        continue

    for a, b in zip(filtered, filtered[1:]):
        src = osm_to_internal[a]
        dst = osm_to_internal[b]
        lat1, lon1 = handler.node_coords[a]
        lat2, lon2 = handler.node_coords[b]
        w = haversine_m(lat1, lon1, lat2, lon2)

        edges_out.append((src, dst, w))
        if not oneway:
            edges_out.append((dst, src, w))

with open(NODES_CSV, "w", newline="", encoding="utf-8") as f:
    writer = csv.writer(f)
    writer.writerow(["node_id", "osm_id", "lat", "lon"])
    writer.writerows(nodes_out)

with open(EDGES_CSV, "w", newline="", encoding="utf-8") as f:
    writer = csv.writer(f)
    writer.writerow(["src", "dst", "weight"])
    writer.writerows(edges_out)

print(f"Done. nodes={len(nodes_out)}, edges={len(edges_out)}")
