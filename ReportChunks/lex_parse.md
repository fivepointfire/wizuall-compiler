Absolutely! Here is a **deep-dive, technical, implementation-focused report** on the lexer and parser for your WizuAll compiler, with explicit details, rationale, and code-level design decisions.

---

# WizuAll Compiler: Lexer and Parser Implementation – Technical Report

---

## 1. Lexer (Flex) – Implementation Details

### 1.1. Token Specification

- **Keywords:**  
  Defined as reserved words in the `.l` file, e.g.:
  ```
  "if"        return IF;
  "else"      return ELSE;
  "while"     return WHILE;
  "for"       return FOR;
  "plot"      return PLOT;
  // ... and all visualization and built-in function names
  ```
  These are mapped to unique token types in the parser.

- **Identifiers:**  
  ```
  [a-zA-Z_][a-zA-Z0-9_]*    { yylval.str = strdup(yytext); return ID; }
  ```
  - All variable and function names are case-sensitive.
  - The value is stored in `yylval.str` for use in the parser.

- **Numbers:**  
  ```
  [0-9]+(\.[0-9]+)?         { yylval.num = atof(yytext); return NUMBER; }
  ```
  - Both integers and floating-point numbers are supported.
  - All numbers are parsed as `double` for downstream uniformity.

- **Strings:**  
  ```
  \"([^\\\"]|\\.)*\"        { yylval.str = strdup(yytext); return STRING; }
  ```
  - Double-quoted strings, with support for escaped quotes and characters.

- **Operators and Delimiters:**  
  Each operator and delimiter is mapped to a unique token, e.g.:
  ```
  "=="    return EQ;
  "!="    return NEQ;
  "<="    return LEQ;
  ">="    return GEQ;
  "="     return ASSIGN;
  "+"     return PLUS;
  "-"     return MINUS;
  "*"     return TIMES;
  "/"     return DIVIDE;
  "("     return LPAREN;
  ")"     return RPAREN;
  "["     return LBRACKET;
  "]"     return RBRACKET;
  "{"     return LBRACE;
  "}"     return RBRACE;
  ","     return COMMA;
  ";"     return SEMICOLON;
  ```

### 1.2. Comments and Whitespace

- **Comments:**  
  ```
  "//".*    /* skip single-line comments */
  ```
  - Entire line is ignored.

- **Whitespace:**  
  ```
  [ \t\r\n]+    /* skip whitespace, but update line/column counters */
  ```
  - Whitespace is ignored, but line numbers are incremented for error reporting.

### 1.3. Error Handling

- Any unrecognized character triggers:
  ```
  .    { fprintf(stderr, "Illegal character '%c' at line %d\n", yytext[0], yylineno); exit(1); }
  ```
  - Immediate termination with a clear error message.

### 1.4. Token Value Passing

- Uses a `%union` in the Bison file to allow `yylval` to carry either `double` (for numbers) or `char*` (for identifiers/strings).
- All tokens that carry values (ID, NUMBER, STRING) set `yylval` appropriately.

### 1.5. Integration

- The lexer is compiled to `lex.yy.c` and linked with the parser.
- The lexer only returns tokens; all structure-building is deferred to the parser.

---

## 2. Parser (Bison) – Implementation Details

### 2.1. Grammar Structure

- **Start Symbol:**  
  ```
  Program : StatementList
  ```
  - The root of the AST is a program node containing a list of statements.

- **Statements:**  
  ```
  Statement
      : Assignment SEMICOLON
      | FunctionCall SEMICOLON
      | VisualizationCall SEMICOLON
      | ControlStructure
      ;
  ```
  - Each statement is parsed and added to the AST.

- **Assignment:**  
  ```
  Assignment : ID ASSIGN Expression
  ```
  - Produces an AST node with the variable name and the expression subtree.

- **Expressions:**  
  - Arithmetic, logical, and comparison expressions are handled with precedence and associativity rules.
  - Example:
    ```
    Expression
        : Expression PLUS Expression
        | Expression MINUS Expression
        | Expression TIMES Expression
        | Expression DIVIDE Expression
        | LPAREN Expression RPAREN
        | NUMBER
        | ID
        | FunctionCall
        | VectorLiteral
        | MatrixLiteral
        ;
    ```
  - Each operation creates a binary or unary AST node.

- **Function Calls:**  
  ```
  FunctionCall : ID LPAREN ArgListOpt RPAREN
  ```
  - `ArgListOpt` allows for zero or more arguments.
  - Arguments are parsed as a linked list of AST nodes.

- **Visualization Calls:**  
  ```
  VisualizationCall : VizFuncName LPAREN VizArgListOpt RPAREN
  ```
  - `VizFuncName` is a set of reserved words (plot, histogram, etc.).
  - `VizArgListOpt` supports both positional and keyword arguments.

- **Vectors and Matrices:**  
  ```
  VectorLiteral : LBRACKET ExprList RBRACKET
  MatrixLiteral : LBRACKET VectorList RBRACKET
  ```
  - Vectors are lists of expressions.
  - Matrices are lists of vectors.

- **Control Structures:**  
  - **If-Else:**
    ```
    IfElse : IF LPAREN Expression RPAREN LBRACE StatementList RBRACE ELSE LBRACE StatementList RBRACE
    ```
  - **While:**
    ```
    While : WHILE LPAREN Expression RPAREN LBRACE StatementList RBRACE
    ```
  - **For:**
    ```
    For : FOR LPAREN Assignment SEMICOLON Expression SEMICOLON Assignment RPAREN LBRACE StatementList RBRACE
    ```
  - Each control structure creates a corresponding AST node with condition and body subtrees.

### 2.2. AST Construction

- **Node Types:**  
  - Each grammar rule’s semantic action allocates and populates a C struct representing an AST node.
  - Node types include: PROGRAM, ASSIGNMENT, FUNCTION_CALL, VIZ_CALL, IF_ELSE, WHILE_LOOP, FOR_LOOP, VECTOR_LITERAL, MATRIX_LITERAL, NUMBER, ID, STRING, BINARY_OP, etc.

- **Memory Management:**  
  - All AST nodes are heap-allocated.
  - Linked lists are used for argument and statement lists.

- **Type Information:**  
  - Each node carries type information (e.g., number, vector, matrix, string) for semantic checks and code generation.

### 2.3. Error Handling

- **Syntax Errors:**  
  - The parser uses Bison’s error token to recover from errors and continue parsing, allowing multiple errors to be reported in one run.
  - Custom error messages include line and column numbers.

- **Semantic Checks:**  
  - Some semantic checks (e.g., correct number of function arguments) are performed in the parser; others are deferred to later passes.

### 2.4. Operator Precedence and Associativity

- Precedence is declared in the Bison file:
  ```
  %left PLUS MINUS
  %left TIMES DIVIDE
  %left EQ NEQ LT GT LEQ GEQ
  ```
  - This ensures correct parsing of expressions like `a + b * c`.

### 2.5. Extensibility

- **Adding New Functions/Visualizations:**  
  - To add a new function, add its keyword to the lexer, its grammar rule to the parser, and its code generation logic to the backend.
  - The grammar is modular, so new constructs can be added with minimal changes.

### 2.6. Integration with Code Generation

- After parsing, the AST is passed to the code generation module.
- The parser does not generate code directly; it only builds the AST.

### 2.7. AST Printing

- After parsing, a recursive function traverses the AST and prints its structure to the terminal for debugging and verification.

---

## 3. Notable Implementation Decisions

- **All numbers as double:**  
  - Ensures consistency and avoids type errors in mathematical and plotting operations.
- **Keyword arguments for visualizations:**  
  - Enables flexible, Pythonic syntax for plots.
- **AST-based design:**  
  - Decouples parsing from code generation, making the compiler easier to maintain and extend.
- **Error recovery:**  
  - Parser attempts to recover from errors to provide a better user experience.
- **Line/column tracking:**  
  - Lexer and parser both track line/column for precise error messages.

---

## 4. Example: Parsing a Visualization Call

Given:
```wzl
plot(x, y, title="Line Plot", xlabel="X Axis", ylabel="Y Axis");
```

- **Lexer output:**  
  `PLOT LPAREN ID COMMA ID COMMA ID ASSIGN STRING COMMA ID ASSIGN STRING COMMA ID ASSIGN STRING RPAREN SEMICOLON`
- **Parser actions:**  
  - Recognizes as a `VisualizationCall`.
  - Builds a VIZ_CALL AST node with:
    - Function: `plot`
    - Positional args: `x`, `y`
    - Keyword args: `title="Line Plot"`, `xlabel="X Axis"`, `ylabel="Y Axis"`

---

## 5. Code Snippet: AST Node for Assignment

```c
typedef struct {
    char* var_name;
    ASTNode* expr;
} AssignmentNode;
```
- Created in the semantic action for `Assignment : ID ASSIGN Expression`.

---

## 6. Summary Table

| Component | Tool   | Key Features/Choices |
|-----------|--------|---------------------|
| Lexer     | Flex   | Regex-based, double support, error handling, line/col tracking |
| Parser    | Bison  | LALR(1) grammar, AST construction, operator precedence, error recovery, extensibility |

---

## 7. Conclusion

The WizuAll lexer and parser are engineered for robustness, extensibility, and clarity. The design choices—such as full floating-point support, AST-based architecture, and detailed error reporting—enable a powerful and user-friendly DSL for data analysis and visualization.

---

If you want actual code excerpts, a sample AST printout, or a diagram of the AST structure, just ask!
