import matplotlib.pyplot as plt
x = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
y = [2.5, 4, 5.5, 4, 5, 6, 7, 8, 9, 10.5]
z = [3, 6, 7, 8, 9, 10, 11, 12, 13, 14]
k = x[3:7]
print(k)
plt.plot(x, y, color='blue', linestyle='-', marker='', markersize=5, linewidth=2)
plt.title("Line Plot")
plt.xlabel("X Axis")
plt.ylabel("Y Axis")
plt.grid(True)
plt.show()
plt.hist(y, bins=10, color='skyblue', edgecolor='black', density=False)
plt.title("Histogram Example")
plt.xlabel("Value")
plt.ylabel("Frequency")
plt.grid(True)
plt.show()
matrix = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]
plt.imshow(matrix, cmap='viridis', interpolation='nearest', aspect='auto')
plt.title("Heatmap Example")
plt.xlabel("X")
plt.ylabel("Y")
plt.colorbar()
plt.show()
plt.bar(x, y, color='orange')
plt.title("Bar Chart Example")
plt.xlabel("Categories")
plt.ylabel("Values")
plt.grid(axis='y')
plt.show()
values = [4, 2.5, 2, 10]
labels = ["Apples", "Bananas", "Cherries", "Dates"]
plt.pie(values, labels=labels)
plt.title('Pie Chart')
plt.show()
plt.scatter(x, y, color='blue', marker='o', s=100, alpha=0.6)
plt.title("Scatter Example")
plt.xlabel("X")
plt.ylabel("Y")
plt.grid(True)
plt.show()
t_labels = ["hello", "World", "May"]
plt.boxplot([x, y, z], tick_labels=t_labels, notch=False, vert=True, patch_artist=True)
plt.title("Box Plot Example")
plt.xlabel("Groups")
plt.ylabel("Values")
plt.grid(True)
plt.show()
