Here is a clear and organized syntax documentation for your WizuAll language, split into two sections:

---

# WizuAll Syntax Documentation

---

## 1. Non-Visualization Functions

### **Variable Assignment**
```wzl
variable = value;
```
- Assigns a value (number, vector, matrix, or function result) to a variable.

### **Vectors and Matrices**
```wzl
v = [1, 2, 3, 4];
m = [
    [1, 2],
    [3, 4]
];
```
- Square brackets `[]` for vectors.
- Nested brackets for matrices.

### **Built-in Functions**

#### **avg**
```wzl
result = avg(vector);
```
- Returns the average of the elements in `vector`.

#### **sort**
```wzl
result = sort(vector);
```
- Returns a sorted version of `vector`.

#### **reverse**
```wzl
result = reverse(vector);
```
- Returns a reversed version of `vector`.

#### **slice**
```wzl
result = slice(vector, start, end);
```
- Returns a subvector from `start` (inclusive) to `end` (exclusive).

#### **transpose**
```wzl
result = transpose(matrix);
```
- Returns the transpose of a matrix.

#### **runningSum**
```wzl
result = runningSum(vector);
```
- Returns the cumulative sum of the vector.

#### **pairwiseCompare**
```wzl
result = pairwiseCompare(vector);
```
- Returns a vector of differences between consecutive elements.

#### **paretoSet**
```wzl
result = paretoSet(vector);
```
- Returns the set of unique elements in the vector.

### **Print**
```wzl
print("message", variable, ...);
```
- Prints messages and/or variable values to the terminal.

### **Control Structures**

#### **If-Else**
```wzl
if (condition) {
    // statements
} else {
    // statements
}
```

#### **While Loop**
```wzl
while (condition) {
    // statements
}
```

#### **For Loop**
```wzl
for (i = start; i <= end; i = i + 1) {
    // statements
}
```

---

## 2. Visualization Functions

**The following syntax and options are based on your `examples/test.wzl` file.**

### **Line Plot**
```wzl
plot(x, y, title="Line Plot", xlabel="X Axis", ylabel="Y Axis");
```
- **x, y:** Vectors of data.
- **title:** (optional) Plot title.
- **xlabel:** (optional) X-axis label.
- **ylabel:** (optional) Y-axis label.

---

### **Histogram**
```wzl
histogram(y, title="Histogram Example", xlabel="Value", ylabel="Frequency");
```
- **y:** Vector of data.
- **title:** (optional) Plot title.
- **xlabel:** (optional) X-axis label.
- **ylabel:** (optional) Y-axis label.

---

### **Heatmap**
```wzl
matrix = [
    [1, 2, 3],
    [4, 5, 6],
    [7, 8, 9]
];
heatmap(matrix, title="Heatmap Example", xlabel="X", ylabel="Y");
```
- **matrix:** 2D array of data.
- **title:** (optional) Plot title.
- **xlabel:** (optional) X-axis label.
- **ylabel:** (optional) Y-axis label.

---

### **Bar Chart**
```wzl
barchart(x, y, title="Bar Chart Example", xlabel="Categories", ylabel="Values");
```
- **x, y:** Vectors of data.
- **title:** (optional) Plot title.
- **xlabel:** (optional) X-axis label.
- **ylabel:** (optional) Y-axis label.

---

### **Pie Chart**
```wzl
values = [4.0, 2.5, 2, 10];
labels = ["Apples", "Bananas", "Cherries", "Dates"];
piechart(values, labels=labels);
```
- **values:** Vector of data.
- **labels:** (optional) Vector of string labels.

---

### **Scatter Plot**
```wzl
scatter(x, y, title="Scatter Example", xlabel="X", ylabel="Y");
```
- **x, y:** Vectors of data.
- **title:** (optional) Plot title.
- **xlabel:** (optional) X-axis label.
- **ylabel:** (optional) Y-axis label.

---

### **Box Plot**
```wzl
t_labels = ["hello", "World", "May"];
boxplot([x, y, z], title="Box Plot Example", xlabel="Groups", ylabel="Values", tick_labels=t_labels);
```
- **[x, y, z]:** List of vectors (multiple data series).
- **title:** (optional) Plot title.
- **xlabel:** (optional) X-axis label.
- **ylabel:** (optional) Y-axis label.
- **tick_labels:** (optional) Vector of string labels for each group.

---

**Note:**  
- All function calls end with a semicolon `;`.
- Keyword arguments are supported for all visualization functions as shown above.
- You can mix and match options as needed, following the examples.

---

Let me know if you want this in a different format (Markdown, PDF, etc.) or want to add more examples!
