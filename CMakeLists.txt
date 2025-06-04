cmake_minimum_required(VERSION 3.16)

project(tlou2_disasm VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source")

set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/source/ui")

file(GLOB_RECURSE SOURCE_FILES
    "${SOURCE_DIR}/*.cpp"
    "${SOURCE_DIR}/*.c"
    "${SOURCE_DIR}/*.h"
    "${SOURCE_DIR}/*.hpp"
    "${SOURCE_DIR}/*.ui"
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(tlou2_disasm
        MANUAL_FINALIZATION
        ${SOURCE_FILES}
    )
    target_compile_options(tlou2_disasm PRIVATE
        $<$<CONFIG:Release>:-O1 -mavx512f>
        $<$<CONFIG:RelWithDebInfo>:-O2 -g -DNDEBUG>
        $<$<CONFIG:Debug>:-O0 -g>
    )

# Define target properties for Android with Qt 6 as:
# set_property(TARGET tlou2_disasm APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
# ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(tlou2_disasm SHARED
            ${SOURCE_FILES}
        )

    # Define properties for Android with Qt 5 after find_package() calls as:
    # set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(tlou2_disasm
            ${SOURCE_FILES}
        )
    endif()
endif()

target_link_libraries(tlou2_disasm PRIVATE Qt${QT_VERSION_MAJOR}::Widgets stdc++fs)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.tlou2_disasm)
endif()

set_target_properties(tlou2_disasm PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS tlou2_disasm
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(tlou2_disasm)
endif()
