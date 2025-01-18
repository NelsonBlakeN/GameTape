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

const bool saveImage = true;

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

std::string grabAndProcessArea(std::tuple<int, int, int, int> area, QScreen *screen, std::string imageName)
{
    QRect captureArea(std::get<0>(area), std::get<1>(area), std::get<2>(area), std::get<3>(area));
    QPixmap screenshot = screen->grabWindow(0, captureArea.x(), captureArea.y(), captureArea.width(), captureArea.height());
    QImage qImage = screenshot.toImage();

    QString fileName = QString::fromStdString(imageName + ".png");
    if (saveImage)
    {
        if (!screenshot.save(fileName))
        {
            std::cerr << "Failed to save screenshot." << std::endl;
            return "";
        }
    }

    Pix *pixImage = nullptr;
    if (qImage.format() != QImage::Format_ARGB32)
    {
        qImage = qImage.convertToFormat(QImage::Format_ARGB32);
    }
    pixImage = pixCreate(qImage.width(), qImage.height(), 32);
    uint8_t *pixData = reinterpret_cast<uint8_t *>(pixGetData(pixImage));
    memcpy(pixData, qImage.bits(), qImage.sizeInBytes());

    tesseract::TessBaseAPI api;
    if (api.Init(nullptr, "eng"))
    {
        std::cerr << "Failed to initialize Tesseract." << std::endl;
        pixDestroy(&pixImage);
        return "";
    }

    api.SetImage(pixImage);

    char *text = api.GetUTF8Text();
    if (text)
    {
        std::string result(text);
        std::replace(result.begin(), result.end(), '\n', ' ');
        delete[] text;
        return result;
    }
    else
    {
        std::cerr << "Failed to extract text from image." << std::endl;
        return "";
    }

    pixDestroy(&pixImage);
    api.End();
}
