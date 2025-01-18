#ifndef SCREENSHOT_UTILS_HPP
#define SCREENSHOT_UTILS_HPP

#include <string>
#include <vector>
#include <QScreen>

double calculateMSE(const QImage &img1, const QImage &img2);

std::string grabAndProcessArea(std::tuple<int, int, int, int> area, QScreen *screen, std::string imageName = "screenshot");

#endif // SCREENSHOT_UTILS_HPP
