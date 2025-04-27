# WizuAll Compiler – Detailed Setup & Usage Guide

## 1. Prerequisites

You need a **Linux** system or **Windows Subsystem for Linux (WSL)**.  
The following tools and libraries must be installed:

- **Python 3** (for running generated code)
- **pip** (Python package manager)
- **make** (build tool)
- **gcc** (C compiler)
- **flex** (lexical analyzer generator)
- **bison** (parser generator)

### Install System Packages

Open your terminal and run:

```bash
sudo apt update
sudo apt install python3 python3-pip make gcc flex bison
```

## 2. Install Required Python Libraries

Install the Python libraries used by the generated code:

```bash
pip3 install matplotlib numpy
```

## 3. Build the WizuAll Compiler

In your project directory, run:

```bash
make clean
make
```

This will compile the WizuAll compiler executable.

## 4. Compile a WizuAll Program

To compile a WizuAll source file (for example, `examples/tc6.wzl`):

```bash
./wizuall_compiler < examples/tc6.wzl
```

**What happens when you run this command:**
- The compiler will print the Abstract Syntax Tree (AST) for your WizuAll program to the terminal.  
  This helps you visualize the structure of your code and debug any issues.
- After printing the AST, the compiler will generate a Python file named `output.py` in the same directory.

## 5. Run the Generated Python Code

Execute the generated Python code with:

```bash
python3 output.py
```

## 6. What to Expect

- **AST Output:**  
  When you run the compiler, the AST for your WizuAll program will be displayed in the terminal before code generation.
- **Plots:**  
  If your WizuAll script contains visualization commands (like `plot`, `barchart`, `scatter`, etc.), the generated Python code will display the corresponding plots using matplotlib.
- **Print Statements:**  
  Any `print` commands in your WizuAll script will output results directly to the terminal/command line.
- **Warnings:**  
  Any matplotlib warnings (such as those about non-interactive backends) are automatically suppressed in the generated code.

## 7. Example Workflow

```bash
# 1. Build the compiler
make clean
make

# 2. Compile a WizuAll file (prints AST and generates output.py)
./wizuall_compiler < examples/visualization_tests.wzl

# 3. Run the generated Python code
python3 output.py
```
This Creates Your plots in the directory of your wzl code 


## 8. Notes

- You can replace `examples/visualization_tests.wzl` with any other `.wzl` file you wish to compile.
- Ensure all dependencies are installed before running the compiler or generated code.
- No shell scripts are required; all steps are manual and transparent.

