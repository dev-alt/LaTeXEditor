cmake_minimum_required(VERSION 3.16)
project(LaTeXEditor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Set the deployment target to macOS 14.3 (Sonoma)
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum OS X deployment version")

# Ensure we're building for x86_64
set(CMAKE_OSX_ARCHITECTURES "x86_64")

# Find the Qt installation
if(DEFINED ENV{Qt6_DIR} AND NOT DEFINED Qt6_DIR)
    set(Qt6_DIR $ENV{Qt6_DIR})
endif()

find_package(Qt6 COMPONENTS Widgets REQUIRED)

if (Qt6WebEngineWidgets_FOUND)
    add_definitions(-DQT_WEBENGINEWIDGETS_LIB)
    message(STATUS "Qt6WebEngineWidgets found, enabling web-based preview")
else ()
    message(STATUS "Qt6WebEngineWidgets not found, falling back to QTextBrowser for preview")
endif ()

set(SOURCE_FILES
        src/main.cpp
        src/models/DocumentModel.cpp
        src/models/Theme.cpp
        src/views/MainWindow.cpp
        src/views/LatexToolbar.cpp
        src/views/PreviewWindow.cpp
        src/controllers/EditorController.cpp
        src/controllers/FileController.cpp
        src/controllers/LatexToolbarController.cpp
        src/controllers/PreviewController.cpp
        src/utils/LaTeXHighlighter.cpp
        src/utils/ThemeManager.cpp
        resources.qrc
)

if(APPLE)
    set(ICON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/latex_editor_icon.icns)
    if(EXISTS ${ICON_FILE})
        set(MACOSX_BUNDLE_ICON_FILE latex_editor_icon.icns)
        set_source_files_properties(${ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
        list(APPEND SOURCE_FILES ${ICON_FILE})
    else()
        message(WARNING "Icon file ${ICON_FILE} not found. The application will be built without an icon.")
    endif()
endif()

add_executable(LaTeXEditor MACOSX_BUNDLE ${SOURCE_FILES})

target_link_libraries(LaTeXEditor PRIVATE Qt6::Widgets)

if (Qt6WebEngineWidgets_FOUND)
    target_link_libraries(LaTeXEditor PRIVATE Qt6::WebEngineWidgets)
endif ()

if(APPLE)
    set_target_properties(LaTeXEditor PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_GUI_IDENTIFIER com.yourcompany.LaTeXEditor
            MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
            MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
    )
endif()

# Install rules
install(TARGETS LaTeXEditor
        BUNDLE DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Include a custom command for macdeployqt if on macOS
if(APPLE)
    find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${Qt6_DIR}/../../../bin")
    if(MACDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET LaTeXEditor POST_BUILD
                COMMAND "${MACDEPLOYQT_EXECUTABLE}"
                "$<TARGET_FILE_DIR:LaTeXEditor>/../.."
                -always-overwrite
                COMMENT "Running macdeployqt..."
        )
    endif()
endif()
