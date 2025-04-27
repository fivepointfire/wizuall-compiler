import matplotlib.pyplot as plt

# Sample data
x = [1, 2, 3, 4, 5]
y = [2, 4, 6, 8, 10]
z = [3, 5, 7, 9, 11]

# Test scatter plot
plt.figure()
plt.scatter(x, y, color='blue', marker='o', s=100, alpha=0.6)
plt.title('Scatter Plot Test')
plt.xlabel('X-axis')
plt.ylabel('Y-axis')
plt.grid(True)
plt.show()

# Test box plot
plt.figure()
plt.boxplot([x, y, z], notch=False, vert=True, patch_artist=True, labels=['X', 'Y', 'Z'])
plt.title('Box Plot Test')
plt.xlabel('Groups')
plt.ylabel('Values')
plt.grid(True)
plt.show()

# Test pie chart with correct autopct format
plt.figure()
values = [30, 20, 25, 15, 10]
plt.pie(values, colors=['gold', 'skyblue', 'lightgreen'], autopct='%.1f%%', startangle=140)
plt.title('Pie Chart Test')
plt.show() 