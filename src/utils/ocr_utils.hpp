#ifndef OCR_UTILS_HPP
#define OCR_UTILS_HPP

#include <string>

std::string getLatestModelName();

std::string getText(
    Pix *pixImage,
    bool saveImage,
    tesseract::PageSegMode psm,
    std::string imageName);

#endif // OCR_UTILS_HPP