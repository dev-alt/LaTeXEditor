cmake_minimum_required(VERSION 3.16)
project(LaTeXEditor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find the Qt installation
if(DEFINED ENV{Qt6_DIR} AND NOT DEFINED Qt6_DIR)
    set(Qt6_DIR $ENV{Qt6_DIR})
endif()

find_package(Qt6 COMPONENTS Widgets REQUIRED)

set(SOURCE_FILES
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
    list(APPEND SOURCE_FILES ${APP_ICON_MACOSX})
endif()

if(APPLE)
    add_executable(LaTeXEditor MACOSX_BUNDLE ${SOURCE_FILES})
else()
    add_executable(LaTeXEditor ${SOURCE_FILES})
endif()

target_link_libraries(LaTeXEditor PRIVATE Qt6::Widgets)

if(APPLE)
    set_target_properties(LaTeXEditor PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER com.yourcompany.LaTeXEditor
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    )
endif()
