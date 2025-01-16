#ifndef SCREENSHOT_UTILS_HPP
#define SCREENSHOT_UTILS_HPP

#include <string>
#include <vector>
#include <QScreen>

double calculateMSE(const QImage &img1, const QImage &img2);

bool areImagesSimilar(const QImage &image1, const QImage &image2);

std::string grabAndProcessArea(std::tuple<int, int, int, int> area, QScreen *screen, std::string imageName = "screenshot");

// Function to capture a screenshot of the current window and save it to a file
bool captureScreenshot(const std::string &filename);

// Function to capture a screenshot of a specific region of the screen
bool captureScreenshotRegion(int x, int y, int width, int height, const std::string &filename);

// Utility function to save the screenshot in a specific format
bool saveScreenshot(const std::vector<unsigned char> &pixelData, int width, int height, const std::string &filename);

#endif // SCREENSHOT_UTILS_HPP
