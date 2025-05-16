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

/**
 * Functions that only deal with retrieving, returning, or processing images
 */

double calculateMSE(const QImage &img1, const QImage &img2)
{
    if (img1.size() != img2.size())
    {
        std::cout << "Size mismatch" << std::endl;
        return -1;
    }

    double mse = 0.0;
    int width = img1.width();
    int height = img1.height();

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            QColor color1 = img1.pixelColor(x, y);
            QColor color2 = img2.pixelColor(x, y);

            int rDiff = color1.red() - color2.red();
            int gDiff = color1.green() - color2.green();
            int bDiff = color1.blue() - color2.blue();

            mse += (rDiff * rDiff + gDiff * gDiff + bDiff * bDiff);
        }
    }
    mse /= (width * height * 3);
    return mse;
}

void SavePixToFile(Pix *pix, const char *filename)
{
    pixWrite(filename, pix, IFF_PNG);
}

Pix *BinarizeImageOcr(Pix *pixImage)
{
    // Convert image to grayscale
    Pix *grayImage = pixConvertRGBToLuminance(pixImage);

    Pix *binaryImage = pixThresholdToBinary(grayImage, 125);

    pixDestroy(&grayImage); // Clean up the grayscale image
    return binaryImage;
}

/*
 * Save the screenshot to a folder with a specific name
 * @param screenshot The screenshot to save
 * @param folderName The name of the folder to save the image in
 * @param fileName The name of the file to save the image as
 * @return
 */
Pix *captureScreenshot(
    std::tuple<int, int, int, int> area,
    bool saveImage)
{
    QScreen *screen = QApplication::primaryScreen();
    QRect captureArea(std::get<0>(area), std::get<1>(area), std::get<2>(area), std::get<3>(area));
    QPixmap screenshot = screen->grabWindow(0, captureArea.x(), captureArea.y(), captureArea.width(), captureArea.height());
    QImage qImage = screenshot.toImage();

    Pix *pixImage = nullptr;
    if (qImage.format() != QImage::Format_ARGB32)
    {
        qImage = qImage.convertToFormat(QImage::Format_ARGB32);
        if (qImage.format() != QImage::Format_ARGB32)
        {
            std::cerr << "Failed to convert image format to ARGB32." << std::endl;
        }
    }

    pixImage = pixCreate(qImage.width(), qImage.height(), 32);

    uint8_t *pixData = reinterpret_cast<uint8_t *>(pixGetData(pixImage));
    const uint8_t *qImageData = qImage.bits();

    for (int y = 0; y < qImage.height(); ++y)
    {
        for (int x = 0; x < qImage.width(); ++x)
        {
            int index = (y * qImage.bytesPerLine()) + (x * 4); // Assuming 4 bytes per pixel
            pixData[index + 0] = qImageData[index + 2];        // Blue
            pixData[index + 1] = qImageData[index + 1];        // Green
            pixData[index + 2] = qImageData[index + 0];        // Red
            pixData[index + 3] = qImageData[index + 3];        // Alpha
        }
    }

    return pixImage;
}
