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
            fi
        done
    fi
done