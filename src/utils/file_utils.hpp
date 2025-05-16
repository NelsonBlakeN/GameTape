#ifndef FILE_UTILS_HPP
#define FILE_UTILS_HPP

#include <QDir>
#include <QPixmap>
#include <iostream>
#include <random>
#include <sstream>
#include <leptonica/allheaders.h>

// Function to generate a random string of given length
std::string generateRandomString(size_t length);

// Function to create a folder with the specified name
bool createFolder(const QString &folderName);

// Function to save an image to a specified folder with a given name
bool saveImageToFolder(const QPixmap &image, const QString &folderName, const QString &imageName);

bool saveImageToFolder(Pix *image, const QString &folderName, const QString &imageName);

#endif // FILE_UTILS_HPP