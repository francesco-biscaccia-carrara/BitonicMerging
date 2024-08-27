import pandas as pd
import matplotlib.pyplot as plt

commtimes = pd.read_csv('comm_times.csv')
exetimes = pd.read_csv('exe_times.csv')

commtimes = commtimes.drop(columns=['MergeSort'])
exetimes = exetimes.drop(columns=['MergeSort'])

CCR = (exetimes.set_index('INST_SIZE') - commtimes.set_index('INST_SIZE')) / exetimes.set_index('INST_SIZE') * 100

plt.figure(figsize=(10, 6))

for col in CCR.columns:
    plt.plot(CCR.index, CCR[col], marker='o', label=col)

plt.xlabel('Instance Size')
plt.ylabel('$CCR(P)$ [%]')
plt.title('$CCR(P)$ (Computing-over-Communication Ratio) for each instance size')
plt.legend()
plt.grid(True)

plt.tight_layout()
plt.savefig("plot/CCR.png")
plt.savefig("plot/CCR.pgf", backend="pgf")