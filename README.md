# WizuAll Compiler – Quick Start & Usage Guide

## 1. Prerequisites

- **Linux** or **WSL** environment
- **Python 3** installed (`python3 --version`)
- **pip** for Python 3 (`pip3 --version`)
- **make**, **gcc**, **flex**, and **bison** installed

### Install System Packages

```bash
sudo apt update
sudo apt install python3 python3-pip make gcc flex bison
```

## 2. Install Required Python Libraries

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

## 4. Importing Data from JSON/CSV Files

You can import data variables directly from JSON or CSV files using the following syntax at the top of your `.wzl` file:

```wzl
import "data.json";
import "data.csv";
```
- After import, all top-level keys (for JSON) or columns (for CSV) become variables in your WizuAll code.
- Example usage:
  ```wzl
  import "data.json";
  plot(x, y);
  ```
- **Important:** Place all data files (e.g., `data.json`, `data.csv`) in the main project directory (the same directory where you run the compiler and where `output.py` is generated).

## 5. Compile and Run a WizuAll Program

To compile a WizuAll source file (for example, `examples/tc6.wzl`):

```bash
./wizuall_compiler examples/tc6.wzl
```

This will generate a Python file named `output.py` in the main directory.

To run the generated Python code:

```bash
python3 output.py
```

## 6. Plot Output Directory and File Naming

- All plot images are now saved in a subfolder called `plots/` in the main directory.
- The images are named `plot_<runid>_<counter>.png` for each run, where `<runid>` is a unique timestamp for that run and `<counter>` is the plot number (e.g., `plot_1717171717_1.png`).
- This ensures that plots from different runs will **not** overwrite each other.
- The `plots` directory is created automatically if it does not exist.
- **Tip:** You can easily identify which plots belong to which run by their `<runid>` value.

## 7. What to Expect

- **AST Output:**
  When you run the compiler, the AST for your WizuAll program will be displayed in the terminal before code generation.
- **Plots:**
  All plots are saved as PNG files in the `plots/` directory.
- **Print Statements:**
  Any `print` commands in your WizuAll script will output results directly to the terminal/command line.
- **Warnings:**
  Any matplotlib warnings (such as those about non-interactive backends) are automatically suppressed in the generated code.

## 8. Example Workflow

```bash
# 1. Build the compiler
make clean
make

# 2. Compile a WizuAll file (prints AST and generates output.py)
./wizuall_compiler < examples/tc6.wzl

# 3. Run the generated Python code
python3 output.py

# 4. Check the plots/ directory for generated plot images
ls plots/
```

## 9. Notes

- You can replace `examples/tc6.wzl` with any other `.wzl` file you wish to compile.
- Ensure all dependencies are installed before running the compiler or generated code.
- No shell scripts are required; all steps are manual and transparent.

---

**Enjoy using WizuAll for your data analysis and visualization needs!**

