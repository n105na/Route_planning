import csv
import math

file = "results/csv/results.csv"

queries = {}

with open(file, "r") as f:
    reader = csv.DictReader(f)

    for row in reader:
        q = row["query"]
        dist = float(row["distance"])

        if q not in queries:
            queries[q] = dist

total = len(queries)
unreachable = sum(1 for d in queries.values() if math.isinf(d) or d > 1e20)

print("Total queries:", total)
print("Unreachable queries:", unreachable)
print("Percentage:", (unreachable / total) * 100, "%")