#ifndef WINDOW_UTILS_HPP
#define WINDOW_UTILS_HPP

#include <string>

// Function to create a window with specified dimensions
bool createWindow(const std::string &title, int width, int height);

// Function to resize the current window
bool resizeWindow(int width, int height);

// Function to move the window to a specific position on the screen
void moveWindow(const std::string &windowTitle, int x, int y, int width, int height);

#endif // WINDOW_UTILS_HPP
