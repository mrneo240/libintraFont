#!/bin/bash

# Check if the template file exists
if [ ! -f "mips-allegrex-psp.template" ]; then
  echo "Error: Template file 'mips-allegrex-psp.template' not found."
  exit 1
fi

# Read the template file content
template_content=$(cat "mips-allegrex-psp.template")

# Replace '${PSPDEV}' with the actual value of the environment variable
actual_value="${PSPDEV}"
output_content=$(echo "$template_content" | sed "s|\${PSPDEV}|$actual_value|g")

# Create the output file
echo "$output_content" > mips-allegrex-psp
