#include <QApplication>
#include <QCursor>
#include <QPoint>
#include <QTimer>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    std::cout << "Move your mouse to the desired area and press Ctrl+C to stop." << std::endl;

    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, []()
                     {
        QPoint pos = QCursor::pos(); // Get current cursor position
        std::cout << "Mouse position: X=" << pos.x() << ", Y=" << pos.y() << "     \r" << std::flush; });

    timer.start(100); // Update every 100 milliseconds

    return app.exec(); // Enter Qt event loop
}
