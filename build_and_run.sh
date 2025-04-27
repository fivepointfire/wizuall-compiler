#!/bin/bash
set -e

if [ $# -ne 1 ]; then
    echo "Usage: $0 <wizuall_file.wzl>"
    exit 1
fi

WZL_FILE="$1"

# Step 1: Run the compiler on the input WizuAll file
./wizuall_compiler < "$WZL_FILE"

# Step 2: Run the generated Python code
python3 output.py