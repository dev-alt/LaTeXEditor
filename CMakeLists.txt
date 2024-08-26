cmake_minimum_required(VERSION 3.28)
project(LaTeXEditor)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "C:/Qt/6.7.2/mingw_64")

find_package(Qt6 COMPONENTS Widgets REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(LaTeXEditor
        src/main.cpp
        src/models/DocumentModel.cpp
        src/views/MainWindow.cpp
        src/controllers/EditorController.cpp
        src/LaTeXHighlighter.cpp
        resources.qrc
)

target_link_libraries(LaTeXEditor PRIVATE Qt6::Widgets)