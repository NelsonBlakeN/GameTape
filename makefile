CXX = g++
CXXFLAGS = -std=c++17 -fPIC
INCLUDES = -I /opt/homebrew/opt/qt/include \
		   -I /opt/homebrew/opt/qt/include/QtCore \
		   -I /opt/homebrew/opt/qt/include/QtGui \
		   -I /opt/homebrew/opt/qt/include/QtWidgets \
		   -I /opt/homebrew/include/tesseract \
		   -I /opt/homebrew/include/leptonica \
		   -I /opt/homebrew/include
FRAMEWORKS = -F /opt/homebrew/opt/qt/lib \
			 -framework QtCore \
			 -framework QtGui \
			 -framework QtWidgets \
			 -framework ApplicationServices

# Directories
SRC_DIR = src
UTILS_DIR = src/utils
BUILD_DIR = build

LIBRARIES = -L /opt/homebrew/lib -ltesseract -lleptonica

TARGET = $(BUILD_DIR)/app
SOURCES = $(SRC_DIR)/main.cpp \
			$(UTILS_DIR)/screenshot_utils.cpp \
			$(UTILS_DIR)/window_utils.cpp \
			$(UTILS_DIR)/string_utils.cpp

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(FRAMEWORKS) $(LIBRARIES) -o $(TARGET) $(SOURCES) $(TESSERACT_LIBS) $(LEPTONICA_LIBS)

clean:
	rm -f $(TARGET)