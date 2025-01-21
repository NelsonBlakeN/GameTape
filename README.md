# GameTape: Get data faster

## Description

Stop manually writing down recruits yourself.

EA College Football 25's 35 recruit limit means making some hard decisions about who to track and who to take off. It's hard to remember who that fallback player was now that your first choice committed somewhere else, but you had to take them off so you could scout some guys at a different position. And when you did go to scout some new guys, you end up adding a bunch of guys that you forgot you had already scouted, because the recruit menu gives no indication of who you've already seen. Now you have to go through the removal animation, go back to the full list (which has reset the last filters you had) and try again.

Enter GameTape. GameTape works alongside game streaming to ingest everything about recruiting that you can't remember yourself. Start by streaming the game to a computer, then as you scroll through the menu, GameTape will make note of every player's name, position, archetype, star value, and class designation. This data is saved to a CSV file that you can build on however you want. Track player's gem status, their offer status, or notes about their potential, ratings, and abilities. Spend your time making the important decisions, and leave the data collection to GameTape.

## System Requirements

### Known working environments
Currently I run this program with the configuration below, and the code makes assumptions about the location of the data based on these factors. Future improvements could be made to make the program more agnostic to the environment.
- MacBook pro 14-inch with M3 Pro
- macOS 14.0
- External 1080p minitor (assigned to the right of the computer)
- Remote connection to Windows computer using TeamViewer, which is streaming the Xbox in full screen

## Getting Started

### Prerequisites

To build and run this application, you'll need the following dependencies:

* A C++17 compatible compiler (g++)
* Qt Framework (Core, GUI, and Widgets components)
* Tesseract OCR library
* Leptonica imaging library
* Homebrew (for macOS users)

### Installation (macOS)

1. Install Homebrew if you haven't already:

    ```bash
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    ```

2. Install Qt:

    ```bash
    brew install qt
    ```

3. Install Tesseract and Leptonica:

    ```bash
    brew install tesseract leptonica
    ```

### Verify Installation

You can verify that the dependencies are properly installed by running these commands:

1. Check g++ version:

    ```bash
    g++ --version
    ```

2. Check Qt installation

    ```bash
    qmake --version
    ```

3. Check Tesseract installation

    ```bash
    tesseract --version
    ```

4. Verify library locations
    * Qt Should be installed in `/opt/homebrew/opt/qt`
    * Tesseract headers should be in `/opt/homebrew/include/tesseract`
    * Leptonica headers should be in `/opt/homebrew/include/leptonica`

### Building the Application

Once all dependencies are installed, you can build the application by running the following from the project root directory:

```bash
make
```

The compiled executable will be created in the `build` directory.
