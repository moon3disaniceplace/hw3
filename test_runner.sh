#!/bin/bash

# Directories
input_dir="input"
expected_dir="expected"
output_dir="output"
program="./hw3"

# Create output directory if it doesn't exist
mkdir -p "$output_dir"

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color
LIGHT_BLUE="\033[1;34m"
RESET="\033[0m"

# Counters for summary
total_tests=0
passed_tests=0

# Iterate over each input file
for input_file in "$input_dir"/*.in; do
    # Get the base name of the test (e.g., 1_valid)
    test_name=$(basename "$input_file" .in)

    # Expected and output file paths
    expected_file="$expected_dir/$test_name.exp"
    output_file="$output_dir/$test_name.out"

    # Run the program and redirect output
    "$program" < "$input_file" > "$output_file" 2>&1

    # Check if the expected file exists
    if [[ -f "$expected_file" ]]; then
        # Compare the output file with the expected file
        if diff -q "$output_file" "$expected_file" > /dev/null; then
            echo -e "$test_name: ${GREEN}PASS${NC}"
            ((passed_tests++))
        else
            echo -e "$test_name: ${RED}FAIL${NC}"
        fi
    else
        echo -e "${RED}Expected file not found for test: $test_name${NC}"
    fi

    # Increment total tests counter
    ((total_tests++))
done

# Summary
echo -e "\nSummary: $passed_tests out of $total_tests tests passed."
if [[ $passed_tests -eq $total_tests ]]; then
    echo -e "${GREEN}ALL TESTS PASSED!${NC}"
fi
