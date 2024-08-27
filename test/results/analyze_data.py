import re
import numpy as np

file_names = ["st.out","pt_2.out","pt_4.out","pt_8.out","pt_16.out","pt_32.out"]

for file_name in file_names:
    exe_times = {}
    comm_times = {}

    with open("raw_data/"+file_name, 'r') as file:
        current_seed = None
        for line in file:
            seed_match = re.match(r"----(\d+)-----", line)
            if seed_match:
                current_seed = int(seed_match.group(1))
                exe_times[current_seed] = []
                comm_times[current_seed] = []
            elif current_seed is not None:
                data = line.split(',')
                exe_times[current_seed].append(float(data[0]))
                if  len(data) > 1:
                    comm_times[current_seed].append(float(data[1]))

    print(f"From file: {file_name}")
    for seed, times in exe_times.items():
        mean_time = np.mean(times)
        var_time = np.var(times)
        print(f"Seed: {seed}, ExeTime Mean: {mean_time:.6f},Variance: {var_time:.6f}")
    for seed, times in comm_times.items():
        if len(times) > 0 :
            mean_time = np.mean(times)
            var_time = np.var(times)
            print(f"Seed: {seed}, CommTime Mean: {mean_time:.6f},Variance: {var_time:.6f}")
    print(f"------------")
