#!/bin/bash

PARENT_DIR="data/images"

if [ -n "$1" ]; then
    TARGET_FOLDER="$PARENT_DIR/$1"
    if [ -d "$TARGET_FOLDER" ]; then
        FOLDERS=("$TARGET_FOLDER")
    else
        echo "Error: Directory $TARGET_FOLDER does not exist."
        exit 1
    fi
else
    FOLDERS=("$PARENT_DIR"/*)
fi

for FOLDER in "${FOLDERS[@]}"; do
    if [ -d "$FOLDER" ]; then
        FOLDER_NAME=$(basename "$FOLDER")
        echo "Processing folder: $FOLDER_NAME"
        for IMAGE_FILE in "$FOLDER"/*.png; do
            # Check if the current item is a file
            if [ -f "$IMAGE_FILE" ]; then
                IMAGE_BASENAME=$(basename "$IMAGE_FILE")
                TEXT_FILE_PATH="${IMAGE_FILE%.*}.txt"

                # Check if the corresponding text file exists
                if [ ! -f "$TEXT_FILE_PATH" ]; then
                    echo "$FOLDER_NAME" > "$TEXT_FILE_PATH"
                    echo "Generated text file for $IMAGE_FILE"
                fi

                # Generate box files
                /opt/homebrew/bin/tesseract "$IMAGE_FILE" "${IMAGE_FILE%.*}" --psm 7 -l eng batch.nochop makebox
                if [ $? -ne 0 ]; then
                    echo "Error generating box files for $IMAGE_FILE"
                    continue
                fi

                # Check for the generated box file
                BOX_FILE="${IMAGE_FILE%.*}.box"
                if [ ! -f "$BOX_FILE" ]; then
                    echo "Box file not found: $BOX_FILE"
                fi

                # Create training files
                /opt/homebrew/bin/tesseract "$IMAGE_FILE" "${IMAGE_FILE%.*}" --psm 7 nobatch box.train
            fi
        done
    fi
done

# Set up variables
TESSDATA_PREFIX="/opt/homebrew/share/tessdata"  # Adjust based on your system
PARENT_DIR="data/images"
OUTPUT_DIR="training_output"
LANG_CODE="eng"
FONT_NAME="custom"

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Step 1: Combine all .tr files into a single training file
echo "Combining training files..."
cat "$PARENT_DIR"/**/*.tr > "$OUTPUT_DIR/all_combined.tr"

if [ ! -s "$OUTPUT_DIR/all_combined.tr" ]; then
    echo "Error: No training data was combined. Check your .tr files."
    exit 1
fi

# Step 2: Generate unicarset file from box files
echo "Generating unicharset..."
/opt/homebrew/bin/unicharset_extractor "$PARENT_DIR"/**/*.box

if [ ! -f "unicharset" ]; then
    echo "Error: unicharset file was not created."
    exit 1
fi

# Step 3: Create starter lstmf files
echo "Creating lstmf files..."
for tr_file in "$PARENT_DIR"/**/*.tr; do
    base_name="${tr_file%.*}"
    img_file="${base_name}.png"
    if [ -f "$img_file" ]; then
        /opt/homebrew/bin/tesseract "$img_file" "$base_name" --psm 7 lstm.train
    else
        echo "Warning: Image file $img_file not found for training file $tr_file"
    fi
done

# Step 4: Create list of lstmf files
find "$PARENT_DIR" -name '*.lstmf' > "$OUTPUT_DIR/all_files.txt"

# Step 5: Begin fine tuning
echo "Fine-tuning the model..."
/opt/homebrew/bin/lstmtraining \
    --model_output "$OUTPUT_DIR/fine_tuned" \
    --traineddata "$TESSDATA_PREFIX/$LANG_CODE.traineddata" \
    --continue_from "data/eng.lstm" \
    --train_listfile "$OUTPUT_DIR/all_files.txt" \
    --max_iterations 400 \
    --target_error_rate 0.01 \
    # --debug_interval -1

if [ $? -ne 0 ]; then
    echo "Error: Training failed."
    exit 1
fi

# Step 6: Combine into final .traineddata file
echo "Creating final traineddata file..."
# First, create a checkpoint with the correct file extension
cp "$TESSDATA_PREFIX/$LANG_CODE.traineddata" "$OUTPUT_DIR/${LANG_CODE}.${FONT_NAME}.traineddata"
/opt/homebrew/opt/tesseract/bin/combine_tessdata -o "$OUTPUT_DIR/${LANG_CODE}.${FONT_NAME}.traineddata" "$OUTPUT_DIR/fine_tuned_checkpoint"

if [ $? -eq 0 ]; then
    echo "Success! Created fine-tuned model: $OUTPUT_DIR/${LANG_CODE}.${FONT_NAME}.traineddata"
    echo "To use this model, copy it to your tessdata directory:"
    echo "sudo cp $OUTPUT_DIR/${LANG_CODE}.${FONT_NAME}.traineddata $TESSDATA_PREFIX/"
else
    echo "Error: Failed to create final traineddata file."
    exit 1
fi