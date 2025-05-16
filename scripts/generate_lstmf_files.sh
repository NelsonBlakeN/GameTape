#!/bin/bash

# Check if a specific folder is provided as an argument
if [ $# -eq 1 ]; then
    SPECIFIC_FOLDER="$1"
    PARENT_DIR="data/images/$SPECIFIC_FOLDER"
    if [ ! -d "$PARENT_DIR" ]; then
        echo "Error: Specified folder '$SPECIFIC_FOLDER' does not exist in 'data/images'."
        exit 1
    fi
else
    PARENT_DIR="data/images"
fi


# Set up variables
OUTPUT_DIR="training_output"

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Create starter lstmf files
echo "Creating lstmf files..."
for tr_file in "$PARENT_DIR"/*.tr; do
    base_name="${tr_file%.*}"
    img_file="${base_name}.png"
    if [ -f "$img_file" ]; then
        /opt/homebrew/bin/tesseract "$img_file" "$base_name" --psm 7 lstm.train
    else
        echo "Warning: Image file $img_file not found for training file $tr_file"
    fi
done
