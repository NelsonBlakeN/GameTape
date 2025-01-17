#include "window_utils.hpp"
#include <iostream>
#include <ApplicationServices/ApplicationServices.h>

// Create a new window with the given title and dimensions
bool createWindow(const std::string &title, int width, int height)
{
    // Placeholder logic for creating a window (actual implementation depends on the library you're using)
    std::cout << "Creating window: " << title << " with size " << width << "x" << height << std::endl;

    // In practice, you'd use a graphics library (e.g., SDL, GLFW, Qt) here to create the window
    return true;
}

// Resize the current window to the specified width and height
bool resizeWindow(int width, int height)
{
    // Logic to resize the window (mockup)
    std::cout << "Resizing window to " << width << "x" << height << std::endl;

    // Actual implementation would interact with the OS or graphics library
    return true;
}

// Move the window to the specified screen coordinates
void moveWindow(const std::string &windowTitle, int x, int y, int width, int height)
{
    CFArrayRef windowList = CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly, kCGNullWindowID);
    if (!windowList)
    {
        std::cerr << "Failed to get window list." << std::endl;
        return;
    }

    bool windowFound = false;
    for (CFIndex i = 0; i < CFArrayGetCount(windowList); ++i)
    {
        CFDictionaryRef windowInfo = (CFDictionaryRef)CFArrayGetValueAtIndex(windowList, i);
        CFStringRef windowOwnerName = (CFStringRef)CFDictionaryGetValue(windowInfo, kCGWindowOwnerName);
        CFStringRef windowName = (CFStringRef)CFDictionaryGetValue(windowInfo, kCGWindowName);

        char windowOwnerNameBuffer[256];
        char windowNameBuffer[256];
        if (windowOwnerName && CFStringGetCString(windowOwnerName, windowOwnerNameBuffer, sizeof(windowOwnerNameBuffer), kCFStringEncodingUTF8) &&
            windowName && CFStringGetCString(windowName, windowNameBuffer, sizeof(windowNameBuffer), kCFStringEncodingUTF8))
        {
            std::string ownerName(windowOwnerNameBuffer);
            std::string name(windowNameBuffer);

            if (name.find(windowTitle) != std::string::npos)
            {
                windowFound = true;

                int64_t windowID = 0;
                CFNumberRef windowNumber = (CFNumberRef)CFDictionaryGetValue(windowInfo, kCGWindowNumber);
                CFNumberGetValue(windowNumber, kCFNumberSInt64Type, &windowID);

                CGRect newBounds = CGRectMake(x, y, width, height);

                AXUIElementRef systemWideElement = AXUIElementCreateSystemWide();
                AXUIElementRef appElement = nullptr;

                pid_t pid = 0;
                CFNumberRef pidNumber = (CFNumberRef)CFDictionaryGetValue(windowInfo, kCGWindowOwnerPID);
                if (pidNumber)
                {
                    CFNumberGetValue(pidNumber, kCFNumberSInt32Type, &pid);
                    appElement = AXUIElementCreateApplication(pid);
                }

                if (appElement)
                {
                    AXUIElementRef windowElement = nullptr;
                    CFArrayRef windows = nullptr;

                    AXUIElementCopyAttributeValue(appElement, kAXWindowsAttribute, (CFTypeRef *)&windows);
                    if (windows && CFArrayGetCount(windows) > 0)
                    {
                        windowElement = (AXUIElementRef)CFArrayGetValueAtIndex(windows, 0);
                        CFRetain(windowElement);

                        AXValueRef positionValue = AXValueCreate(static_cast<AXValueType>(kAXValueCGPointType), &newBounds.origin);
                        AXValueRef sizeValue = AXValueCreate(static_cast<AXValueType>(kAXValueCGSizeType), &newBounds.size);

                        if (positionValue && sizeValue)
                        {
                            AXError positionError = AXUIElementSetAttributeValue(windowElement, kAXPositionAttribute, positionValue);
                            AXError sizeError = AXUIElementSetAttributeValue(windowElement, kAXSizeAttribute, sizeValue);

                            if (positionError != kAXErrorSuccess)
                            {
                                std::cerr << "Failed to set position. Error: " << positionError << std::endl;
                            }

                            if (sizeError != kAXErrorSuccess)
                            {
                                std::cerr << "Failed to set size. Error: " << sizeError << std::endl;
                            }
                        }
                        else
                        {
                            std::cerr << "No position or size value" << std::endl;
                        }

                        if (positionValue)
                            CFRelease(positionValue);
                        if (sizeValue)
                            CFRelease(sizeValue);
                        if (windows)
                            CFRelease(windows);
                        CFRelease(windowElement);
                    }
                    CFRelease(appElement);
                }
                else
                {
                    std::cerr << "No app element" << std::endl;
                }

                if (systemWideElement)
                    CFRelease(systemWideElement);
                break;
            }
        }
    }

    if (!windowFound)
    {
        std::cerr << "Window with title '" << windowTitle << "' not found." << std::endl;
    }

    CFRelease(windowList);
}
