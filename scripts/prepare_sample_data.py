import csv
from math import sqrt

nodes = [
    (0, 1000, 48.0000, 2.0000),
    (1, 1001, 48.0000, 2.0010),
    (2, 1002, 48.0010, 2.0000),
    (3, 1003, 48.0010, 2.0010),
    (4, 1004, 48.0020, 2.0000),
    (5, 1005, 48.0020, 2.0010),
]

coords = {n[0]: (n[2], n[3]) for n in nodes}

def dist(a, b):
    (x1, y1), (x2, y2) = coords[a], coords[b]
    return sqrt((x1-x2)**2 + (y1-y2)**2) * 100000

edges = [
    (0, 1, dist(0,1)), (1, 0, dist(0,1)),
    (0, 2, dist(0,2)), (2, 0, dist(0,2)),
    (1, 3, dist(1,3)), (3, 1, dist(1,3)),
    (2, 3, dist(2,3)), (3, 2, dist(2,3)),
    (2, 4, dist(2,4)), (4, 2, dist(2,4)),
    (3, 5, dist(3,5)), (5, 3, dist(3,5)),
    (4, 5, dist(4,5)), (5, 4, dist(4,5)),
    (1, 2, dist(1,2) * 1.2),
]

with open('data/sample/nodes_small.csv', 'w', newline='', encoding='utf-8') as f:
    w = csv.writer(f)
    w.writerow(['node_id', 'osm_id', 'lat', 'lon'])
    w.writerows(nodes)

with open('data/sample/edges_small.csv', 'w', newline='', encoding='utf-8') as f:
    w = csv.writer(f)
    w.writerow(['src', 'dst', 'weight'])
    w.writerows(edges)

with open('data/queries/queries_small.csv', 'w', newline='', encoding='utf-8') as f:
    w = csv.writer(f)
    w.writerow(['src', 'dst'])
    w.writerow([0, 5])
    w.writerow([1, 4])
    w.writerow([2, 5])
    w.writerow([4, 1])

print('Sample data written.')
