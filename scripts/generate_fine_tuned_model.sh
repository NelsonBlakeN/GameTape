
# Set up variables
TESSDATA_PREFIX="/opt/homebrew/share/tessdata"  # Adjust based on your system
PARENT_DIR="data/images"
OUTPUT_DIR="training_output"
LANG_CODE="eng"
FONT_NAME="custom"
VERSION_FILE="data/newest_model_version.txt"  # File to track the newest model version

# Ensure the version file exists and initialize it if necessary
if [ ! -f "$VERSION_FILE" ]; then
    echo "Version file not found. Initializing to 0."
    echo "0" > "$VERSION_FILE"
fi

# Read the current version from the version file
MODEL_VERSION=$(cat "$VERSION_FILE")
if [[ ! "$MODEL_VERSION" =~ ^[0-9]+$ ]]; then
    echo "Error: Version file does not contain a valid integer."
    exit 1
fi

echo "Current model version: $MODEL_VERSION"

# Increment the version number and update the version file
NEW_VERSION=$((MODEL_VERSION + 1))
echo "$NEW_VERSION" > "$VERSION_FILE"
echo "Incremented model version to $NEW_VERSION."

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
MODEL_NAME="${LANG_CODE}.${FONT_NAME}V${NEW_VERSION}.traineddata"
echo "Creating final traineddata file with version: $NEW_VERSION..."
# First, create a checkpoint with the correct file extension
cp "$TESSDATA_PREFIX/$LANG_CODE.traineddata" "$OUTPUT_DIR/$MODEL_NAME"
/opt/homebrew/opt/tesseract/bin/combine_tessdata -o "$OUTPUT_DIR/$MODEL_NAME" "$OUTPUT_DIR/fine_tuned_checkpoint"

if [ $? -eq 0 ]; then
    echo "Success! Created fine-tuned model: $OUTPUT_DIR/$MODEL_NAME"
    echo "To use this model, copy it to your tessdata directory:"
    echo "sudo cp $OUTPUT_DIR/$MODEL_NAME $TESSDATA_PREFIX/"
else
    echo "Error: Failed to create final traineddata file."
    exit 1
fi