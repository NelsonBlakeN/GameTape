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

std::map<std::string, std::vector<std::string>> accepted_archs_by_position = {
    {"ATH", {"*"}},
    {"QB (Right)", {"*"}},
    {"OB (Right)", {"*"}},
    {"QB (Left)", {"*"}},
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

std::string get_data_val_for_player(const std::string &position, const std::string &archetype)
{
    // Helper lambda for checking if an element exists in a list
    auto in_list = [](const std::string &value, const std::vector<std::string> &list)
    {
        return std::find(list.begin(), list.end(), value) != list.end();
    };

    if (in_list(position, {"QB (Right)", "QB (Left)"}))
    {
        return "QB";
    }
    else if (position == "HB")
    {
        return "HB";
    }
    else if (position == "WR")
    {
        return "Physical";
    }
    else if (position == "TE")
    {
        return "Vertical threat";
    }
    else if (in_list(position, {"LT", "RT"}) && archetype == "Pass Protector")
    {
        return "Pass protector";
    }
    else if (in_list(position, {"LG", "RG", "C", "LT", "RT"}) && in_list(archetype, {"Power", "Agile"}))
    {
        return "Power/agile";
    }
    else if (in_list(position, {"LE", "RE", "DT"}) && archetype == "Speed Rusher")
    {
        return "Speed rusher";
    }
    else if (in_list(position, {"LE", "RE", "DT", "ROLB", "LOLB", "MLB"}) && archetype == "Run Stopper")
    {
        return "Run stopper";
    }
    else if (in_list(position, {"LOLB", "ROLB"}) && archetype == "Pass Coverage")
    {
        return "Coverage";
    }
    else if (position == "MLB" && in_list(archetype, {"Field General", "Pass Coverage"}))
    {
        return "General/coverage";
    }
    else if (in_list(position, {"CB", "FS", "SS"}))
    {
        return "Zone/hybrid";
    }
    else if (position == "ATH")
    {
        return archetype;
    }
    return ""; // Default return if no match
}

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

std::vector<std::tuple<int, int, int, int>> areas = {
    positionArea,
    archetypeArea,
    nameArea,
    starArea,
    playercardArea,
    classArea};

// TODO: This function may not be needed anymore once the trimming is properly implemented, but needs confirmation
bool archCompare(std::string archResult, std::string archetype)
{
    std::cout << "Prior check: " << (archResult == archetype) << std::endl;
    for (size_t i = 0; i < archetype.size(); ++i)
    {
        if (archetype[i] != archResult[i])
        {
            return false;
        }
    }

    return true;
}

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

    throw std::runtime_error("The first character is not a digit");
}

std::vector<std::string> takeScreenshotAndPerformOCR()
{
    std::vector<std::string> playerData;

    QScreen *screen = QApplication::primaryScreen();
    if (!screen)
    {
        std::cerr << "Failed to access the screen." << std::endl;
        return playerData;
    }

    std::string positionResult = grabAndProcessArea(positionArea, screen, "position");
    std::string archetypeResult = grabAndProcessArea(archetypeArea, screen, "archetype");

    if (positionResult.empty() &&
        (archetypeResult == "Power Back " || archetypeResult == "Elusive Back "))
    {
        positionResult = "HB";
    }

    if (!positionResult.empty() &&
        !archetypeResult.empty())
    {
        positionResult = rtrim(positionResult);
        archetypeResult = rtrim(archetypeResult);

        // TODO: Will this work with [] now that the results are fixed?
        auto archs = accepted_archs_by_position.at(positionResult);

        bool foundArchetype = false;
        for (const auto &arch : archs)
        {
            if (arch == "*" || archCompare(archetypeResult, arch))
            {
                foundArchetype = true;
            }
        }
        // TODO: Will this work now that trimming is introduced?
        // if (std::find(archs.begin(), archs.end(), archetypeResult) != archs.end() ||
        //     std::find(archs.begin(), archs.end(), "*") != archs.end())
        if (foundArchetype)
        {
            std::string nameResult = grabAndProcessArea(nameArea, screen, "name");
            std::replace(nameResult.begin(), nameResult.end(), '\n', ' ');
            nameResult = rtrim(nameResult);
            std::string starResult = grabAndProcessArea(starArea, screen, "star");
            std::string starValue = extractStarValue(starResult);
            std::string classResult = grabAndProcessArea(classArea, screen, "class");

            // TODO: Before compiling data row, use position and archetype with helper function to get appropriate value

            playerData = {
                toTitleCase(nameResult),
                positionResult,
                archetypeResult,
                starValue,
                classResult};

            return playerData;
        }
        else
        {
            std::cout << "Did not find position" << std::endl;
            return playerData;
        }
    }
    else
    {
        std::cout << "Got no position result" << std::endl;
        return playerData;
    }

    return playerData;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    moveWindow("DESKTOP-2MVSFNI (174 915 698)", 1514, 27, 3431 - 1514, 1079 - 27);

    takeScreenshotAndPerformOCR();

    QScreen *screen = QApplication::primaryScreen();
    std::tuple<int, int, int, int> area = nameArea;
    QImage previousImage;
    std::vector<std::string> outputData;
    std::ofstream csvFile("output.csv", std::ios::app);
    if (!csvFile.is_open())
    {
        std::cout << "Could not open the file" << std::endl;
    }
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
            outputData = takeScreenshotAndPerformOCR();

            if (csvFile.is_open() && outputData.size() > 0)
            {
                for (size_t i = 0; i < outputData.size(); ++i)
                {
                    csvFile << outputData[i];
                    if (i < outputData.size() - 1)
                    {
                        csvFile << ",";
                    }
                }
                csvFile << "\n";
                csvFile.flush();
            }
            else
            {
                std::cout << "File was not open" << std::endl;
            }
        }

        QThread::msleep(500);
    }

    csvFile.close();

    return 0;
}
