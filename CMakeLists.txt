cmake_minimum_required(VERSION 3.5)

project(BaicalGUIServer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        includes/launcher.h includes/launcher.cpp
        includes/packethandler.h includes/packethandler.cpp
        includes/chunkhandler.h includes/chunkhandler.cpp
        includes/Trace.h includes/Trace.cpp
        includes/TraceWindow/tracewindow.h includes/TraceWindow/tracewindow.cpp includes/TraceWindow/tracewindow.ui
        includes/FileReader/filereader.h includes/FileReader/filereader.cpp
        includes/FileReader/filechunkshandler.h includes/FileReader/filechunkshandler.cpp
        includes/tracebackupwriter.h includes/tracebackupwriter.cpp
        includes/connectiontimeoutchecker.h includes/connectiontimeoutchecker.cpp
        includes/TraceWindow/tracewindowsettings.h includes/TraceWindow/tracewindowsettings.cpp includes/TraceWindow/tracewindowsettings.ui
        includes/confighandler.h includes/confighandler.cpp
        includes/tracestotxt.h includes/tracestotxt.cpp
        includes/serverstatussender.h includes/serverstatussender.cpp
        icons/icons.qrc
        includes/enumparser.h includes/enumparser.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(BaicalGUIServer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET BaicalGUIServer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(BaicalGUIServer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(BaicalGUIServer
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(BaicalGUIServer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(BaicalGUIServer PRIVATE wsock32 ws2_32)


set_target_properties(BaicalGUIServer PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS BaicalGUIServer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(BaicalGUIServer)
endif()
