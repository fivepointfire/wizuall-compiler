import warnings
warnings.filterwarnings("ignore", category=UserWarning, module="matplotlib")
import matplotlib.pyplot as plt
plot_counter = 1
import time
_wizuall_run_id = int(time.time())
import os
os.makedirs('plots', exist_ok=True)
import csv
with open('data2.csv', 'r') as f:
    reader = csv.DictReader(f)
    _csv_data = list(reader)
    if _csv_data:
        for k in _csv_data[0].keys():
            globals()[k] = [row[k] for row in _csv_data]
plt.plot(x, y, color='blue', linestyle='-', marker='', markersize=5, linewidth=2)
plt.title('Plot')
plt.xlabel('X-axis')
plt.ylabel('Y-axis')
plt.grid(True)
plt.savefig(f'plots/plot_{_wizuall_run_id}_{plot_counter}.png')
plot_counter += 1
plt.clf()
