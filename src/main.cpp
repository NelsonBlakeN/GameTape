#include <QApplication>
#include <QScreen>
#include <QPixmap>
#include <QString>
#include <QDir>
#include <iostream>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cctype>
#include "utils/window_utils.hpp"
#include "utils/string_utils.hpp"
#include "utils/screenshot_utils.hpp"
#include <QThread>
#include "utils/file_utils.hpp"
#include "utils/ocr_utils.hpp"

std::map<std::string, std::vector<std::string>> accepted_archs_by_position = {
    {"ATH", {"*"}},
    {"QB", {"*"}},
    {"QB (Right)", {"*"}},
    {"OB (Right)", {"*"}},
    {"QB (Left)", {"*"}},
    {"OB (Left)", {"*"}},
    {"HB", {"*"}},
    {"WR", {"Physical", "Deep Threat"}},
    {"TE", {"Vertical Threat"}},
    {"LT", {"Power", "Agile", "Pass Protector"}},
    {"LG", {"Power", "Agile"}},
    {"C", {"Power", "Agile"}},
    {"RG", {"Power", "Agile"}},
    {"RT", {"Power", "Agile", "Pass Protector"}},
    {"LE", {"Power Rusher", "Run Stopper"}},
    {"RE", {"Power Rusher", "Run Stopper"}},
    {"DT", {"Run Stopper"}},
    {"LOLB", {"Run Stopper", "Pass Coverage", "Power Rusher"}},
    {"MLB", {"Pass Coverage", "Field General", "Run Stopper"}},
    {"ROLB", {"Run Stopper", "Pass Coverage", "Power Rusher"}},
    {"CB", {"Zone"}},
    {"SS", {"Zone", "Hybrid"}},
    {"FS", {"Zone", "Hybrid"}}};

// Xbox web streaming areas
std::tuple<int, int, int, int> positionAreaWeb =
    {73, 356, 178 - 73, 374 - 356};
std::tuple<int, int, int, int> archetypeAreaWeb =
    {874, 498, 967 - 874, 518 - 498};
std::tuple<int, int, int, int> nameAreaWeb =
    {777, 422, 965 - 777, 460 - 422};
std::tuple<int, int, int, int> starAreaWeb =
    {570, 356, 673 - 570, 373 - 356};
std::tuple<int, int, int, int> playercardAreaWeb =
    {771, 346, 972 - 771, 744 - 346};
std::tuple<int, int, int, int> classAreaWeb =
    {788, 527, 869 - 788, 547 - 527};
std::tuple<int, int, int, int> positionAreaWebBackup =
    {787, 498, 870 - 787, 517 - 498};

// iPhone areas
std::tuple<int, int, int, int> positionAreaPhone = {553, 333, 637 - 553, 352 - 333};
std::tuple<int, int, int, int> archetypeAreaPhone = {642, 333, 740 - 642, 352 - 333};
std::tuple<int, int, int, int> nameAreaPhone = {545, 255, 740 - 545, 294 - 255};
std::tuple<int, int, int, int> starAreaPhone = {330, 186, 432 - 330, 204 - 186};
std::tuple<int, int, int, int> playercardAreaPhone = {534, 173, 746 - 534, 472 - 173};
std::tuple<int, int, int, int> classAreaPhone = {553, 364, 638 - 553, 382 - 364};

// Ultrawide areas
std::tuple<int, int, int, int> positionAreaUW = {3057, 939, 3180 - 3057, 968 - 939};
std::tuple<int, int, int, int> archetypeAreaUW = {3192, 942, 3331 - 3192, 968 - 942};
std::tuple<int, int, int, int> nameAreaUW = {3046, 822, 3331 - 3046, 884 - 822};
std::tuple<int, int, int, int> starAreaUW = {2720, 712, 2882 - 2720, 754 - 712};
std::tuple<int, int, int, int> playercardAreaUW = {3030, 689, 3348 - 3030, 1326 - 689};
std::tuple<int, int, int, int> classAreaUW = {3057, 984, 3185 - 3057, 1017 - 988};

// External monitor areas
std::tuple<int, int, int, int> positionArea = {2968, 520, 3107 - 2968, 552 - 520};
std::tuple<int, int, int, int> archetypeArea = {3115, 521, 3278 - 3115, 552 - 521};
std::tuple<int, int, int, int> nameArea = {2945, 386, 3278 - 2945, 454 - 386};
std::tuple<int, int, int, int> starArea = {2586, 274, 2770 - 2586, 307 - 274};
std::tuple<int, int, int, int> playercardArea = {2943, 260, 3277 - 2943, 938 - 260};
std::tuple<int, int, int, int> classArea = {2964, 572, 3106 - 2964, 603 - 572};

std::string extractStarValue(const std::string &input)
{
    if (input.empty())
    {
        throw std::runtime_error("Input string is empty");
    }

    char firstChar = input[0];
    if (std::isdigit(firstChar))
    {
        return std::string(1, firstChar); // Convert char to a string
    }

    std::cout << "First character is not a digit: " << firstChar << std::endl;
    return "";
}

std::string getValidData(
    std::tuple<int, int, int, int> area,
    tesseract::PageSegMode psm,
    std::function<bool(std::string)> isValid,
    bool saveImage = false,
    std::string imageName = "screenshot",
    std::tuple<int, int, int, int> backupArea = {0, 0, 0, 0})
{
    Pix *img = captureScreenshot(area);
    std::string text = getText(img, saveImage, psm, imageName + "_" + generateRandomString(5));
    std::string textFromBinary = getTextFromBinary(img, saveImage, psm, imageName + "_" + generateRandomString(5));

    if (isValid(text))
    {
        if (saveImage)
        {
            QString folderName = QString::fromStdString(text);
            QString fileName = QString::fromStdString(imageName + ".png");

            if (!saveImageToFolder(img, folderName, fileName))
            {
                std::cerr << "Failed to save image to folder." << std::endl;
            }
        }

        pixDestroy(&img);

        return text;
    }

    std::cout << "Text was not valid, checking binary" << std::endl;

    if (isValid(textFromBinary))
    {
        if (saveImage)
        {
            QString folderName = QString::fromStdString(textFromBinary);
            QString fileName = QString::fromStdString(imageName + ".png");

            if (!saveImageToFolder(img, folderName, fileName))
            {
                std::cerr << "Failed to save image to folder." << std::endl;
            }
        }

        pixDestroy(&img);

        return textFromBinary;
    }

    std::cout << "Invalid data: " << text << std::endl;
    std::cout << "Attempting backup area..." << std::endl;
    if (backupArea != std::make_tuple(0, 0, 0, 0))
    {
        img = captureScreenshot(backupArea);
        text = getText(img, saveImage, psm, imageName + "_" + generateRandomString(5));
        std::cout << "Backup area text: " << text << std::endl;
        textFromBinary = getTextFromBinary(img, saveImage, psm, imageName + "_" + generateRandomString(5));
        std::cout << "Backup text from binary: " << textFromBinary << std::endl;
        if (!isValid(text))
        {
            std::cout << "Invalid data from backup area: " << text << std::endl;

            if (saveImage)
            {
                QString folderName = QString::fromStdString(text);
                QString fileName = QString::fromStdString(imageName + ".png");

                if (!saveImageToFolder(img, folderName, fileName))
                {
                    std::cerr << "Failed to save image to folder." << std::endl;
                }
                else
                {
                    std::cout << "Saved image to folder: " << folderName.toStdString() << "/" << fileName.toStdString() << std::endl;
                }
            }

            if (!isValid(textFromBinary))
            {
                std::cout << "Invalid data from binary backup area: " << textFromBinary << std::endl;
                if (saveImage)
                {
                    QString folderName = QString::fromStdString(text);
                    QString fileName = QString::fromStdString(imageName + ".png");

                    if (!saveImageToFolder(img, folderName, fileName))
                    {
                        std::cerr << "Failed to save image to folder." << std::endl;
                    }
                    else
                    {
                        std::cout << "Saved image to folder: " << folderName.toStdString() << "/" << fileName.toStdString() << std::endl;
                    }
                }
                pixDestroy(&img);
                return "";
            }
            else
            {
                pixDestroy(&img);
                return textFromBinary;
            }

            pixDestroy(&img);
            return "";
        }

        pixDestroy(&img);

        return text;
    }
    else
    {
        std::cout << "No backup area provided." << std::endl;
        pixDestroy(&img);
        return "";
    }

    if (saveImage)
    {
        QString folderName = QString::fromStdString(text);
        QString fileName = QString::fromStdString(imageName + ".png");

        if (!saveImageToFolder(img, folderName, fileName))
        {
            std::cerr << "Failed to save image to folder." << std::endl;
        }
    }

    pixDestroy(&img);

    return text;
}

std::string getValidDataAndTitleCase(
    std::tuple<int, int, int, int> area,
    tesseract::PageSegMode psm,
    std::function<bool(std::string)> isValid,
    bool saveImage = false,
    std::string imageName = "screenshot")
{
    std::string data = getValidData(area, psm, isValid, saveImage, imageName);
    std::string titleCasedData = toTitleCase(data);

    return titleCasedData;
}

std::string getValidDataAndUpperCase(
    std::tuple<int, int, int, int> area,
    tesseract::PageSegMode psm,
    std::function<bool(std::string)> isValid,
    bool saveImage = false,
    std::string imageName = "screenshot",
    std::tuple<int, int, int, int> backupArea = {0, 0, 0, 0})
{
    std::string data = getValidData(area, psm, isValid, saveImage, imageName, backupArea);
    std::transform(data.begin(), data.end(), data.begin(), ::toupper);

    return data;
}

std::string getValidPositionData(bool saveImage)
{
    Pix *positionImg = captureScreenshot(positionAreaWeb);
    std::string positionText = getText(positionImg, saveImage, tesseract::PSM_SINGLE_LINE, "position_" + generateRandomString(5));

    // TODO: Ideally this would only save the image if the text result was valid in the config, but that would require checking it here
    // which isn't really part of the responsibility of this function
    // Theoretically, the function could be expanded to only return the text if the value is found in the config, and null otherwise.
    // This would allow the output data to check for "getValidPositionData," and if that was null, do not proceed with the rest of the data
    if (saveImage)
    {
        QString folderName = QString::fromStdString(positionText);
        QString fileName = QString::fromStdString("position_" + generateRandomString(5) + ".png");

        if (!saveImageToFolder(positionImg, folderName, fileName))
        {
            std::cerr << "Failed to save image to folder." << std::endl;
        }
    }

    auto it = accepted_archs_by_position.find(positionText);
    if (it == accepted_archs_by_position.end())
    {
        std::cout << "Position not found: " << positionText << std::endl;
        return "";
    }

    return positionText;
}

std::vector<std::string> getOutputData()
{
    std::vector<std::string> outputData;

    std::string genericPositionResultUpper = getValidDataAndUpperCase(
        positionAreaWeb,
        tesseract::PSM_SINGLE_LINE,
        [](const std::string &text)
        {
            std::string upperText = text;
            std::transform(upperText.begin(), upperText.end(), upperText.begin(), ::toupper);

            auto it = accepted_archs_by_position.find(upperText);
            if (it == accepted_archs_by_position.end())
            {
                std::cout << "Position not found: " << text << std::endl;
                return false;
            }

            return true;
        },
        true,
        "position_" + generateRandomString(5),
        positionAreaWebBackup);
    std::cout << "New output generic position result upper: " << genericPositionResultUpper << std::endl;

    std::string genericArchetypeResultTitle = getValidDataAndTitleCase(
        archetypeAreaWeb,
        tesseract::PSM_SINGLE_LINE,
        [&genericPositionResultUpper](const std::string &text)
        {
            if (genericPositionResultUpper.empty())
            {
                std::cout << "Generic position result upper is empty" << std::endl;
                return false;
            }

            auto archs = accepted_archs_by_position.find(genericPositionResultUpper)->second;
            if (std::find(archs.begin(), archs.end(), text) == archs.end() &&
                std::find(archs.begin(), archs.end(), "*") == archs.end())
            {
                std::cout << "Archetype not found: " << text << std::endl;
                return false;
            }

            return true;
        },
        false,
        "archetype_" + generateRandomString(5));
    std::cout << "New output generic archetype result upper: " << genericArchetypeResultTitle << std::endl;

    if (!genericPositionResultUpper.empty() && !genericArchetypeResultTitle.empty())
    {
        std::string nameResult = getValidDataAndTitleCase(
            nameAreaWeb,
            tesseract::PSM_SINGLE_BLOCK,
            [](const std::string &text)
            {
                return !text.empty();
            },
            false,
            "name_" + generateRandomString(5));
        std::cout << "New output name result: " << nameResult << std::endl;
        std::string starResult = getValidDataAndUpperCase(
            starAreaWeb,
            tesseract::PSM_SINGLE_LINE,
            [](const std::string &text)
            {
                return !text.empty();
            },
            false,
            "star_" + generateRandomString(5));
        std::string starValue = extractStarValue(starResult);
        std::cout << "New output star result: " << starValue << std::endl;
        std::string classResult = getValidDataAndUpperCase(
            classAreaWeb,
            tesseract::PSM_SINGLE_LINE,
            [](const std::string &text)
            {
                return !text.empty();
            },
            false,
            "class_" + generateRandomString(5));
        std::cout << "New output class result: " << classResult << std::endl;

        std::transform(
            genericPositionResultUpper.begin(),
            genericPositionResultUpper.end(),
            genericPositionResultUpper.begin(),
            ::toupper);

        outputData = {
            nameResult,
            genericPositionResultUpper,
            genericArchetypeResultTitle,
            starValue,
            classResult};
    }
    else
    {
        std::cout << "Position or archetype values were empty" << std::endl;
        std::cout << "Position: " << genericPositionResultUpper << std::endl;
        std::cout << "Archetype: " << genericArchetypeResultTitle << std::endl;
        return outputData;
    }

    return outputData;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    std::string windowName = mode == "phone" ? "iPhone Mirroring" : "DESKTOP-2MVSFNI (174 915 698)";
    // windowName = "Xbox Series X | Xbox Remote Play on  Xbox.com";
    windowName = "EA SPORTS™ College Football 25 | Xbox Cloud Gaming (Beta) on  Xbox.com";

    bool result = moveWindow(windowName, 0, 40, 1008, 982);
    if (!result)
    {
        std::cerr << "Failed to move the window." << std::endl;
        return 1;
    }

    std::cout << "Current model name: " << getLatestModelName() << std::endl;

    QScreen *screen = QApplication::primaryScreen();
    std::tuple<int, int, int, int> area = nameAreaWeb;
    QImage previousImage;
    std::vector<std::string> outputData;
    std::vector<std::string> newOutputData;
    std::ofstream csvFile;
    while (true)
    {
        QRect captureArea(std::get<0>(area), std::get<1>(area), std::get<2>(area), std::get<3>(area));
        QPixmap screenshot = screen->grabWindow(0, captureArea.x(), captureArea.y(), captureArea.width(), captureArea.height());
        QImage qImage = screenshot.toImage();
        if (previousImage.isNull())
        {
            previousImage = qImage;
            continue;
        }
        if (calculateMSE(previousImage, qImage) > 100)
        {
            previousImage = qImage;
            newOutputData = getOutputData();

            if (newOutputData.size() > 0)
            {
                // TODO: Move csv management to file utils
                csvFile.open("output.csv", std::ios::app);
                if (!csvFile.is_open())
                {
                    std::cout << "Could not open the file" << std::endl;
                }
                else
                {
                    for (size_t i = 0; i < newOutputData.size(); ++i)
                    {
                        csvFile << newOutputData[i];
                        if (i < newOutputData.size() - 1)
                        {
                            csvFile << ",";
                        }
                    }
                    csvFile << "\n";
                    csvFile.flush();
                    csvFile.close();
                }
            }
            else
            {
                std::cout << "No data to write" << std::endl;
            }
        }

        QThread::msleep(500);
    }

    csvFile.close();

    return 0;
}
