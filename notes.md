Certainly! Here is a highly detailed, structured report outline and content for your WizuAll compiler project, covering all the requested sections. You can copy this into a document editor and further format it as needed.

---

# Page 1: Title

**WizuAll Compiler Project**  
**A Domain-Specific Language for Data Analysis and Visualization**  
**Comprehensive Design, Implementation, and Refinement Report**  
*Prepared by: [Your Name/Team]*  
*Date: [Insert Date]*

---

# Page 2: Index

1. Title  
2. Index  
3. Introduction  
4. Design  
   - 4.1 Grammar  
   - 4.2 Preprocessor  
   - 4.3 Semantics  
   - 4.4 Scanner  
5. Parser  
6. Syntax-Directed Translation (SDT)  
7. Target Code Generation (Python Integration for Plotting)  
8. Refinement (Error Handling)  
9. References

---

# Page 3: Introduction

The WizuAll project is a custom compiler for a domain-specific language (DSL) designed to simplify data analysis and visualization. The language allows users to write concise, readable scripts for data manipulation, mathematical computation, and the generation of a wide variety of plots (line, bar, scatter, pie, box, heatmap, etc.) using Python’s matplotlib.  
This report details the design, implementation, and refinement of the WizuAll compiler, including its grammar, preprocessor, semantic analysis, scanner, parser, syntax-directed translation, target code generation, and error handling strategies.

---

# Pages 4-6: Design

## 4.1 Grammar

The grammar of WizuAll is defined using Bison/Yacc. It supports:
- Variable assignments
- Vector and matrix literals
- Mathematical and logical expressions
- Function calls (e.g., `avg`, `sort`, `reverse`, `slice`, `transpose`, `runningSum`, `pairwiseCompare`, `paretoSet`)
- Control structures (`if-else`, `while`, `for`)
- Visualization calls (e.g., `plot`, `histogram`, `heatmap`, `barchart`, `piechart`, `scatter`, `boxplot`, `timeline`)
- Keyword arguments for visualizations

**Example Grammar Snippet:**
```yacc
Program
    : StatementList                { final_ast = createProgramNode($1); }
    ;

Statement
    : Assignment SEMICOLON
    | ControlStructure
    | VisualizationCall SEMICOLON
    | FunctionCall SEMICOLON
    ;
```

## 4.2 Preprocessor

The preprocessor is minimal, as WizuAll is designed to be a scripting language. Comments (`// ...`) are stripped by the lexer. The preprocessor could be extended for macros or includes in future versions.

## 4.3 Semantics

Semantic actions are embedded in the grammar rules. They build an Abstract Syntax Tree (AST) using C structs. Each node type (assignment, function call, visualization, etc.) is represented by a specific struct.  
Semantic checks include:
- Type checking for expressions
- Ensuring correct number and type of arguments for functions and visualizations
- Handling of keyword arguments

## 4.4 Scanner

The scanner (lexer) is implemented using Flex. It recognizes:
- Identifiers
- Numbers (integers and floating point, e.g., `4.1`, `100.2`)
- Strings
- Operators and delimiters
- Keywords

**Key improvements:**
- The NUMBER regex was refined to support only valid floating point and integer numbers.
- The `%union` in the parser was updated to use `double` for floating point support.

---

# Pages 7-8: Parser

The parser is built with Bison/Yacc and uses the grammar described above.  
**Key features:**
- Builds the AST for the entire program.
- Handles operator precedence and associativity.
- Supports both positional and keyword arguments for visualizations.
- Allows expressions as values for keyword arguments (e.g., `labels=labels`).

**Error Handling in Parser:**
- Custom error messages with line and column numbers.
- Graceful recovery from common syntax errors.

**Example:**
```yacc
VizArg
    : ID ASSIGN STRING
        { ... }
    | ID ASSIGN Expression
        { ... }
    | Expression
        { ... }
    ;
```

---

# Pages 9-10: Syntax-Directed Translation (SDT)

SDT is used to translate the AST into Python code.  
**Key points:**
- Each AST node has a corresponding code generation function.
- Visualization nodes generate Python matplotlib code.
- Function calls and expressions are translated to Python equivalents.
- Control structures are mapped to Python `if`, `while`, and `for` statements.

**Example:**
- `boxplot([x, y, z]);` in WizuAll becomes:
  ```python
  plt.boxplot([x, y, z], notch=False, vert=True, patch_artist=True, tick_labels=['Data 1', 'Data 2', 'Data 3'])
  ```

---

# Pages 10-11: Target Code Generation (Python Integration for Plotting)

The code generator outputs Python code that:
- Imports necessary libraries (`matplotlib`, `numpy` as needed).
- Suppresses matplotlib warnings at the very top of the file.
- Handles all visualizations, ensuring correct argument mapping and default values.
- Dynamically generates tick labels for boxplots to match the number of datasets.
- Handles floating point numbers with full precision.
- Supports both interactive and non-interactive environments.

**Special Handling:**
- For `plt.show()` warnings, the generator inserts:
  ```python
  import warnings
  warnings.filterwarnings("ignore", category=UserWarning, module="matplotlib")
  ```
  at the very top of the file.

---

# Remaining Pages: Refinement (Error Handling)

## Error Handling and Refinement

### 1. **Lexer/Parser Errors**
- Custom error messages with line and column numbers.
- Clear feedback for invalid tokens, unexpected input, and mismatched parentheses/brackets.

### 2. **Semantic Errors**
- Checks for undefined variables.
- Ensures correct number and type of arguments for functions and visualizations.
- Reports errors for unsupported operations or invalid argument types.

### 3. **Runtime Errors in Generated Python**
- The generated code is robust against common Python errors (e.g., mismatched tick labels, invalid data types).
- Default values and error checks are inserted where possible (e.g., for boxplot tick labels).

### 4. **User Experience Refinements**
- All warnings from matplotlib are suppressed to avoid confusion in non-interactive environments.
- The code generator ensures that all output is clean and ready for use or further modification.

### 5. **Testing and Validation**
- Multiple test cases were created for both visualization and non-visualization features.
- Edge cases (e.g., floating point parsing, empty vectors, mismatched labels) were specifically tested and handled.

---

# References

- [Flex Manual](https://westes.github.io/flex/manual/)
- [Bison Manual](https://www.gnu.org/software/bison/manual/)
- [Matplotlib Documentation](https://matplotlib.org/stable/contents.html)
- [Python Official Documentation](https://docs.python.org/3/)
- [Project Source Code and Test Cases]

---

**End of Report**

---

If you want this in a specific format (Word, PDF, LaTeX), or want more detail in any section (e.g., code snippets, diagrams, test results), let me know!
