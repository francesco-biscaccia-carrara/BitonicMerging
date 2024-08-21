import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

df = pd.read_csv("refined_data.csv")

x_labels = df['INST_SIZE']

cpu_columns = df.columns[1:]

x = np.arange(len(x_labels))
width = 0.15 

fig, ax = plt.subplots(figsize=(12, 8))

for i, cpu in enumerate(cpu_columns):
    ax.bar(x + i * width, df[cpu], width, label=cpu)

ax.set_xlabel('Instance Size')
ax.set_ylabel('Time (seconds)')
ax.set_title('Execution time')
ax.set_xticks(x + width * (len(cpu_columns) - 1) / 2)
ax.set_xticklabels(x_labels)
ax.legend()

plt.tight_layout()
plt.savefig("Exe_time.png")
plt.savefig("Exe_time.pgf",backend="pgf")