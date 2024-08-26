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

set(SOURCES
        src/main.cpp
        src/models/DocumentModel.cpp
        src/models/Theme.cpp
        src/views/MainWindow.cpp
        src/controllers/EditorController.cpp
        src/utils/LaTeXHighlighter.cpp
        src/utils/ThemeManager.cpp
        resources.qrc
)

if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE latex_editor_icon.icns)
    set(APP_ICON_MACOSX ${CMAKE_CURRENT_SOURCE_DIR}/latex_editor_icon.icns)
    set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

add_executable(LaTeXEditor MACOSX_BUNDLE ${SOURCE_FILES} ${APP_ICON_MACOSX})

target_link_libraries(LaTeXEditor PRIVATE Qt6::Widgets)

if(APPLE)
    set_target_properties(LaTeXEditor PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER com.yourcompany.LaTeXEditor
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    )
endif()
