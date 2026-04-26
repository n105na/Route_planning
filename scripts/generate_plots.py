import os
import csv
import math
import numpy as np
import matplotlib.pyplot as plt

INPUT_FILE = "results/csv/results.csv"
FIG_DIR = "results/figures"

os.makedirs(FIG_DIR, exist_ok=True)

# =========================
# LOAD + FILTER
# =========================
results = {
    "dijkstra": [],
    "astar": [],
    "alt": [],
    "ch": []
}

with open(INPUT_FILE, "r") as f:
    reader = csv.DictReader(f)

    for row in reader:
        algo = row["algo"]
        time = float(row["time_ms"])
        dist = float(row["distance"])

        # ❌ REMOVE invalid paths (CRITICAL)
        if dist <= 0:
            continue
        if math.isinf(dist):
            continue
        if dist > 1e20:  # DBL_MAX protection
            continue

        results[algo].append(time)

# =========================
# CLEAN OUTLIERS
# =========================
def clean(data):
    return [x for x in data if x < 1000]  # remove extreme spikes

cleaned = {k: clean(v) for k, v in results.items()}

# =========================
# STATS
# =========================
stats = {}

for algo, data in cleaned.items():
    stats[algo] = {
        "avg": np.mean(data),
        "p50": np.percentile(data, 50),
        "p95": np.percentile(data, 95),
    }

# =========================
# HISTOGRAM (FIXED)
# =========================
plt.figure()

for algo, data in cleaned.items():
    plt.hist(data, bins=40, alpha=0.5, label=algo)

plt.title("Query Time Distribution (Filtered)")
plt.xlabel("Time (ms)")
plt.ylabel("Frequency")
plt.legend()
plt.xlim(0, 100)  # 🔥 VERY IMPORTANT

plt.savefig(f"{FIG_DIR}/hist.png")
plt.close()

# =========================
# BAR PLOTS
# =========================
algos = list(stats.keys())

def bar_plot(values, title, filename):
    plt.figure()
    plt.bar(algos, values)
    plt.title(title)
    plt.ylabel("ms")
    plt.savefig(f"{FIG_DIR}/{filename}")
    plt.close()

bar_plot([stats[a]["avg"] for a in algos], "Average Time", "avg.png")
bar_plot([stats[a]["p50"] for a in algos], "P50 Time", "p50.png")
bar_plot([stats[a]["p95"] for a in algos], "P95 Time", "p95.png")

# =========================
# LOG SCALE (IMPORTANT)
# =========================
plt.figure()
plt.bar(algos, [stats[a]["avg"] for a in algos])
plt.yscale("log")

plt.title("Average Time (Log Scale)")
plt.ylabel("ms (log)")

plt.savefig(f"{FIG_DIR}/avg_log.png")
plt.close()

# =========================
# PRINT SUMMARY
# =========================
print("\n===== CLEAN RESULTS =====")
for algo in algos:
    print(f"{algo}: avg={stats[algo]['avg']:.3f} ms | p50={stats[algo]['p50']:.3f} | p95={stats[algo]['p95']:.3f}")