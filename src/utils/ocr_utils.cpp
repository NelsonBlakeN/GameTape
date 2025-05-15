#include "screenshot_utils.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <QApplication>
#include <QScreen>
#include <QPixmap>
#include <QString>
#include <QDir>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <QCryptographicHash>
#include "file_utils.hpp"
#include "string_utils.hpp"
#include <tesseract/publictypes.h>
#include "ocr_utils.hpp"
#include "screenshot_utils.hpp"
#include "file_utils.hpp"

/**
 * Functions that are only responsible for getting text from images
 */

std::string getLatestModelName()
{
    const std::string tessdataPath = "/opt/homebrew/share/tessdata";
    const std::string versionFilePath = "data/newest_model_version.txt";
    const std::string langCode = "eng";
    const std::string fontName = "custom";

    // Open the version file
    std::ifstream versionFile(versionFilePath);
    if (!versionFile.is_open())
    {
        throw std::runtime_error("Failed to open version file: " + versionFilePath);
    }

    // Read the version number from the file
    int version;
    versionFile >> version;

    if (versionFile.fail() || version <= 0)
    {
        throw std::runtime_error("Invalid version number in version file: " + versionFilePath);
    }

    // Compose the model name
    std::ostringstream modelNameStream;
    modelNameStream << langCode << "." << fontName << "V" << version;

    return modelNameStream.str();
}

Pix *BinarizeImageOcr(Pix *pixImage)
{
    // Convert image to grayscale
    Pix *grayImage = pixConvertRGBToLuminance(pixImage);

    Pix *binaryImage = pixThresholdToBinary(grayImage, 125);

    pixDestroy(&grayImage); // Clean up the grayscale image
    return binaryImage;
}

std::string getTextFromBinary(
    Pix *pixImage,
    bool saveImage,
    tesseract::PageSegMode psm,
    std::string imageName)
{
    // Get the latest model name
    std::string modelName;
    try
    {
        modelName = getLatestModelName();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return "";
    }

    // Initialize Tesseract with the latest model
    tesseract::TessBaseAPI api;
    if (api.Init("/opt/homebrew/share/tessdata", modelName.c_str()))
    {
        std::cerr << "Failed to initialize Tesseract." << std::endl;
        pixDestroy(&pixImage);
        return "";
    }

    Pix *binaryImage = BinarizeImageOcr(pixImage);

    api.SetPageSegMode(psm);
    api.SetImage(binaryImage);

    char *text = api.GetUTF8Text();
    std::string result = "";
    if (text)
    {
        result = text;
        std::replace(result.begin(), result.end(), '\n', ' ');
        delete[] text;
        result = rtrim(result);

        if (saveImage)
        {
            SavePixToFile(binaryImage, "binary_position_image.png");
        }
    }
    else
    {
        std::cerr << "Failed to extract text from image." << std::endl;
    }

    pixDestroy(&binaryImage);
    api.End();

    return result;
}

std::string getText(
    Pix *pixImage,
    bool saveImage,
    tesseract::PageSegMode psm,
    std::string imageName)
{
    // Get the latest model name
    std::string modelName;
    try
    {
        modelName = getLatestModelName();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return "";
    }

    // Initialize Tesseract with the latest model
    tesseract::TessBaseAPI api;
    if (api.Init("/opt/homebrew/share/tessdata", modelName.c_str()))
    {
        std::cerr << "Failed to initialize Tesseract." << std::endl;
        pixDestroy(&pixImage);
        return "";
    }

    // SavePixToFile(pixImage, "original_image.png");
    Pix *binaryImage = BinarizeImageOcr(pixImage);

    if (saveImage)
    {
        // SavePixToFile(binaryImage, "binary_image.png");

        api.SetPageSegMode(psm);
        api.SetImage(binaryImage);
        char *binaryText = api.GetUTF8Text();
        if (binaryText)
        {
            std::string binaryResult(binaryText);
            std::replace(binaryResult.begin(), binaryResult.end(), '\n', ' ');
            delete[] binaryText;
            binaryResult = rtrim(binaryResult);

            if (imageName.find("class_") != std::string::npos)
            {
                return binaryResult;
            }
        }
    }

    api.SetPageSegMode(psm);
    api.SetImage(pixImage);

    char *text = api.GetUTF8Text();
    std::string result = "";
    if (text)
    {
        result = text;
        std::replace(result.begin(), result.end(), '\n', ' ');
        delete[] text;
        result = rtrim(result);
    }
    else
    {
        std::cerr << "Failed to extract text from image." << std::endl;
    }

    // pixDestroy(&pixImage);
    api.End();

    return result;
}