import re
import numpy as np

file_names = ["st.out","pt_2.out","pt_4.out","pt_8.out","pt_16.out","pt_32.out"]

for file_name in file_names:
    data = {}

    with open("raw_data/"+file_name, 'r') as file:
        current_seed = None
        for line in file:
            seed_match = re.match(r"----(\d+)-----", line)
            if seed_match:
                current_seed = int(seed_match.group(1))
                data[current_seed] = []
            elif current_seed is not None:
                time_match = re.match(r"([\d\.]+) secs", line)
                if time_match:
                    time_value = float(time_match.group(1))
                    data[current_seed].append(time_value)

    print(f"From file: {file_name}")
    for seed, times in data.items():
        mean_time = np.mean(times)
        var_time = np.var(times)
        print(f"Seed: {seed}, Mean: {mean_time:.6f},Variance: {var_time:.6f}")
    print(f"------------")
