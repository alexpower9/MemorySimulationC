#!/bin/bash

# Folder where tests are stored
TEST_DIR="./tests"

# Loop through all the test input files (ignoring _out.txt files)
for input_file in "$TEST_DIR"/*.txt; do
    # Skip output files by only selecting non "_out.txt" files
    if [[ "$input_file" != *_out.txt ]]; then
        # Generate the expected output filename by replacing the input filename with _out.txt
        output_file="${input_file%.txt}_out.txt"

        # Run your program, redirecting output to a temporary file
        ./memory "$input_file" > temp_output.txt

        # Compare the program output to the expected output
        if diff -q temp_output.txt "$output_file" > /dev/null; then
            echo "Test passed for $input_file"
        else
            echo "Test failed for $input_file. See differences below:"
            diff temp_output.txt "$output_file"
        fi

        # Optionally, clean up the temporary output file
        rm temp_output.txt
    fi
done
