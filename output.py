import warnings
warnings.filterwarnings("ignore", category=UserWarning, module="matplotlib")
plot_counter = 1
import time
_wizuall_run_id = int(time.time())
import os
os.makedirs('plots', exist_ok=True)
a = [1, 2, 3, 4, 5]
b = [10, 20, 30, 40, 50]
sum_a = (sum(a) / len(a)) + (sum(b) / len(b))
sorted_b = sorted(b)
reversed_a = list(reversed(a))
sliced_b = b[1:4]
transposed = list(map(list, zip(*[[1, 2], [3, 4]])))
print("sum_a:", sum_a)
print("sorted_b:", sorted_b)
print("reversed_a:", reversed_a)
print("sliced_b:", sliced_b)
print("transposed:", transposed)
