import warnings
warnings.filterwarnings("ignore", category=UserWarning, module="matplotlib")
import matplotlib.pyplot as plt
plot_counter = 1
x = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
y = [2.5, 4, 5.5, 4, 5, 6, 7, 8, 9, 10.5]
z = [3, 6, 7, 8, 9, 10, 11, 12, 13, 14]
values = [4, 2.5, 2, 10]
labels = ["Apples", "Bananas", "Cherries", "Dates"]
plt.pie(values, labels=labels)
plt.title('Pie Chart')
plt.savefig(f'plot_{plot_counter}.png')
plot_counter += 1
plt.clf()
plt.scatter(x, y, color='blue', marker='o', s=100, alpha=0.6)
plt.title("Scatter Example")
plt.xlabel("X")
plt.ylabel("Y")
plt.grid(True)
plt.savefig(f'plot_{plot_counter}.png')
plot_counter += 1
plt.clf()
t_labels = ["hello", "World", "May"]
plt.boxplot([x, y, z], tick_labels=t_labels, notch=False, vert=True, patch_artist=True)
plt.title("Box Plot Example")
plt.xlabel("Groups")
plt.ylabel("Values")
plt.grid(True)
plt.savefig(f'plot_{plot_counter}.png')
plot_counter += 1
plt.clf()
