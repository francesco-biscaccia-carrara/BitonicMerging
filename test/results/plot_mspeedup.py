import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from scipy.stats import linregress

df = pd.read_csv("refined_data.csv")

algorithms = df.columns[2:]

speedups_by_algorithm = {}

for algorithm in algorithms:
    speedups = df["MergeSort"] / df[algorithm]
    speedups_by_algorithm[algorithm] = speedups.mean()


algorithms = list(speedups_by_algorithm.keys())
average_speedups = list(speedups_by_algorithm.values())

x = np.arange(len(algorithms))


fig, ax = plt.subplots(figsize=(12, 8))
ax.scatter(x, average_speedups, color='red', label='')

slope, intercept, r_value, p_value, std_err = linregress(x, average_speedups)
line = slope * x + intercept

ax.plot(x, line, color='grey', linestyle='--',)

ax.set_xlabel('Algorithm')
ax.set_ylabel('Average Speedup')
ax.set_title('Speedup Behaviour')
ax.set_xticks(x)
ax.set_xticklabels(algorithms)

plt.tight_layout()
plt.savefig("Avg_Speedup.png")
plt.savefig("Avg_Speedup.pgf", backend="pgf")