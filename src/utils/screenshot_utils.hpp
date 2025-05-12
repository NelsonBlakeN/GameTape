#ifndef SCREENSHOT_UTILS_HPP
#define SCREENSHOT_UTILS_HPP

#include <string>
#include <vector>
#include <QApplication>
#include <QScreen>
#include <QPixmap>
#include <QString>
#include <leptonica/allheaders.h>
#include <tesseract/publictypes.h> // For tesseract::PageSegMode

double calculateMSE(const QImage &img1, const QImage &img2);

Pix *captureScreenshot(
    std::tuple<int, int, int, int> area,
    bool saveImage = false);

std::string grabAndProcessArea(
    std::tuple<int, int, int, int> area,
    QScreen *screen,
    std::string imageName = "screenshot",
    bool saveImage = true,
    tesseract::PageSegMode psm = tesseract::PSM_SINGLE_BLOCK);

void SavePixToFile(Pix *pix, const char *filename);

#endif // SCREENSHOT_UTILS_HPP
