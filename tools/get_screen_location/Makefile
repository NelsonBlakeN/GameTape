CXX = g++
CXXFLAGS = -std=c++17 -fPIC
INCLUDES = -I /opt/homebrew/opt/qt/include \
           -I /opt/homebrew/opt/qt/include/QtCore \
           -I /opt/homebrew/opt/qt/include/QtGui \
           -I /opt/homebrew/opt/qt/include/QtWidgets

# Frameworks and libraries
FRAMEWORKS = -F /opt/homebrew/opt/qt/lib \
             -framework QtCore \
             -framework QtGui \
             -framework QtWidgets \
             -framework ApplicationServices

# Output executable
TARGET := build/cross_platform_mouse_tracker

# Source file
SRC := src/main.cpp

# Default rule
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(FRAMEWORKS)  -o $(TARGET) $(SRC)

# Rule to clean up generated files
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: all clean
