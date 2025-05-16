#!/bin/bash

PARENT_DIR="data/images"
INVALID_FILES=()

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

get_char_at_position() {
    local string="$1"
    local position="$2"
    echo "$string" | cut -c$((position + 1))
}

validate_box_file() {
    local box_file="$1"
    local expected_text="$2"
    local expected_length="${#expected_text}"

    local line_count=$(wc -l < "$box_file")

    if [ "$line_count" -eq "$expected_length" ]; then
        local needs_update=false
        local temp_file=$(mktemp)
        local curr_pos=0

        while IFS= read -r line; do
            local char="${line:0:1}"
            local rest_of_line="${line:1}"

            local upper_char=$(echo "$char" | tr '[:lower:]' '[:upper:]')
            local expected_char=$(get_char_at_position "$expected_text" "$curr_pos")

            if [[ "$char" =~ [a-z] ]] && [[ "$upper_char" == "$expected_char" ]]; then
                needs_update=true
                echo "$upper_char$rest_of_line" >> "$temp_file"
            else
                echo "$line" >> "$temp_file"
            fi
            curr_pos=$((curr_pos + 1))
        done < "$box_file"

        if [ "$needs_update" = true ]; then
            mv "$temp_file" "$box_file"
            echo "Updated case in $box_file"
        else
            rm "$temp_file"
        fi
    else
        echo "Invalid box file: $box_file"
        INVALID_FILES+=("$box_file")
    fi
}

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
                if [ ! -f "$BOX_FILE" ]; then
                    echo "Generating box file for $IMAGE_FILE"
                    /opt/homebrew/bin/tesseract "$IMAGE_FILE" "${IMAGE_FILE%.*}" --psm 7 -l eng batch.nochop makebox
                    if [ $? -ne 0 ]; then
                        echo "Error generating box files for $IMAGE_FILE"
                        continue
                    fi
                fi

                # Check for the generated box file
                BOX_FILE="${IMAGE_FILE%.*}.box"
                if [ -f "$BOX_FILE" ]; then
                    validate_box_file "$BOX_FILE" "$FOLDER_NAME"
                else
                    echo "Box file not found: $BOX_FILE"
                fi
            fi
        done
    fi
done

# Print invalid files at the end
if [ ${#INVALID_FILES[@]} -gt 0 ]; then
    echo "Invalid box files:"
    printf '%s\n' "${INVALID_FILES[@]}"
fi