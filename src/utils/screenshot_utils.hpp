#ifndef SCREENSHOT_UTILS_HPP
#define SCREENSHOT_UTILS_HPP

#include <string>
#include <vector>
#include <QApplication>
#include <QScreen>
#include <QPixmap>
#include <QString>
#include <leptonica/allheaders.h>
#include <tesseract/publictypes.h>

double calculateMSE(const QImage &img1, const QImage &img2);

Pix *captureScreenshot(
    std::tuple<int, int, int, int> area,
    bool saveImage = false);

void SavePixToFile(Pix *pix, const char *filename);

Pix *BinarizeImageOcr(Pix *pixImage);

#endif // SCREENSHOT_UTILS_HPP
