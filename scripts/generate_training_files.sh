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

                # Create training files
                /opt/homebrew/bin/tesseract "$IMAGE_FILE" "${IMAGE_FILE%.*}" --psm 7 nobatch box.train
            fi
        done
    fi
done