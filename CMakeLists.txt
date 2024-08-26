cmake_minimum_required(VERSION 3.28)
project(LaTeXEditor)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)
include_directories(include)

add_executable(LaTeXEditor
        src/main.cpp
        src/models/DocumentModel.cpp
        src/views/MainWindow.cpp
        src/controllers/EditorController.cpp
)
target_link_libraries(LaTeXEditor PRIVATE Qt6::Widgets)