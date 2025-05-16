cmake_minimum_required(VERSION 3.12)

# Read version from version.txt file
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/version.txt" VERSION_STRING)
string(STRIP "${VERSION_STRING}" VERSION_STRING)

project(cmake-app VERSION ${VERSION_STRING}
        DESCRIPTION "Game Tape"
        LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # For -fPIC

# Get required packages
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

set(HOMEBREW_PREFIX "/opt/homebrew")
set(LEPTONICA_VERSION "1.85.0")

find_library(TESSERACT_LIBRARY
    NAMES tesseract
    PATHS ${HOMEBREW_PREFIX}/lib
    REQUIRED
    NO_DEFAULT_PATH
)

find_library(LEPTONICA_LIBRARY
    NAMES
        leptonica
        libleptonica.dylib
    PATHS
        ${HOMEBREW_PREFIX}/lib
        ${HOMEBREW_PREFIX}/Cellar/leptonica/${LEPTONICA_VERSION}/lib
    REQUIRED
    NO_DEFAULT_PATH
)

find_path(TESSERACT_INCLUDE_DIR
    NAMES tesseract/baseapi.h
    PATHS ${HOMEBREW_PREFIX}/include
    REQUIRED
    NO_DEFAULT_PATH
)

find_path(LEPTONICA_INCLUDE_DIR
    NAMES leptonica/allheaders.h
    PATHS
        ${HOMEBREW_PREFIX}/include
        ${HOMEBREW_PREFIX}/Cellar/leptonica/${LEPTONICA_VERSION}/include
    REQUIRED
    NO_DEFAULT_PATH
)

# Create version header
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/version.h
    @ONLY
)

# Source files
set(SOURCES
    src/main.cpp
    src/utils/screenshot_utils.cpp
    src/utils/window_utils.cpp
    src/utils/string_utils.cpp
    src/utils/file_utils.cpp
    src/utils/ocr_utils.cpp
)

# Generate executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Add include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${TESSERACT_INCLUDE_DIR}
    ${LEPTONICA_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    ${TESSERACT_LIBRARY}
    "${HOMEBREW_PREFIX}/lib/libleptonica.dylib"  # Explicitly link against the dylib
)

if(APPLE)
    # macOS framework
    target_link_libraries(${PROJECT_NAME} PRIVATE
        "-framework ApplicationServices"
    )
endif()

message(STATUS "LEPTONICA_LIBRARY: ${LEPTONICA_LIBRARY}")
message(STATUS "LEPTONICA_INCLUDE_DIR: ${LEPTONICA_INCLUDE_DIR}")
message(STATUS "TESSERACT_LIBRARY: ${TESSERACT_LIBRARY}")
message(STATUS "TESSERACT_INCLUDE_DIR: ${TESSERACT_INCLUDE_DIR}")

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)