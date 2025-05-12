#include <QDir>
#include <QPixmap>
#include <iostream>
#include <random>
#include <sstream>
#include <leptonica/allheaders.h>

std::string generateRandomString(size_t length)
{
    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, characters.size() - 1);

    std::stringstream randomString;
    for (size_t i = 0; i < length; ++i)
    {
        randomString << characters[distribution(generator)];
    }
    return randomString.str();
}

bool createFolder(const QString &folderName)
{
    QDir dir(folderName);
    if (!dir.exists())
    {
        if (!dir.mkpath("."))
        {
            std::cerr << "Failed to create folder: " << folderName.toStdString() << std::endl;
            return false;
        }
    }
    return true;
}

bool saveImageToFolder(Pix *image, const QString &folderName, const QString &imageName)
{
    if (createFolder("data/images/" + folderName))
    {
        QString filePath = "data/images/" + folderName + "/" + imageName;

        // Convert Pix* to format that can be saved
        l_uint8 *data;
        size_t size;

        // Convert to PNG in memory
        if (pixWriteMem(&data, &size, image, IFF_PNG) != 0)
        {
            std::cerr << "Failed to convert image to PNG format" << std::endl;
            return false;
        }

        // Write the PNG data to file
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly))
        {
            free(data);
            std::cerr << "Failed to open file for writing: " << filePath.toStdString() << std::endl;
            return false;
        }

        qint64 written = file.write(reinterpret_cast<char *>(data), size);
        free(data);
        file.close();

        if (written != static_cast<qint64>(size))
        {
            std::cerr << "Failed to write complete image data to: " << filePath.toStdString() << std::endl;
            return false;
        }

        return true;
    }
    return false;
}

bool saveImageToFolder(const QPixmap &image, const QString &folderName, const QString &imageName)
{
    if (createFolder("data/images/" + folderName))
    {
        QString filePath = "data/images/" + folderName + "/" + imageName;
        // QString filePath = imageName;
        if (!image.save(filePath))
        {
            std::cerr << "Failed to save image: " << filePath.toStdString() << std::endl;
            return false;
        }
        // else
        // {
        //     std::cout << "Image saved to: " << filePath.toStdString() << std::endl;
        // }
        return true;
    }
    return false;
}