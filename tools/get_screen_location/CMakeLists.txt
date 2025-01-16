cmake_minimum_required(VERSION 3.10)
project(MouseTracker)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 REQUIRED COMPONENTS Core Gui)

add_executable(mouse_tracker src/main.cpp)
target_link_libraries(mouse_tracker Qt6::Core Qt6::Gui)
