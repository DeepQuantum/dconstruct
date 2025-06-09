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

set(CMAKE_AUTOUIC_SEARCH_PATHS "${SOURCE_DIR}/ui")

file(GLOB_RECURSE ALL_SOURCE_FILES
    "${SOURCE_DIR}/*.cpp"
    "${SOURCE_DIR}/*.c"
    "${SOURCE_DIR}/*.h"
    "${SOURCE_DIR}/*.hpp"
    "${SOURCE_DIR}/*.ui"
)

# Separate main files
set(GUI_MAIN "${SOURCE_DIR}/main_gui.cpp")
set(CLI_MAIN "${SOURCE_DIR}/main_cli.cpp")

# Filter common sources (exclude both mains)
list(REMOVE_ITEM ALL_SOURCE_FILES ${GUI_MAIN} ${CLI_MAIN})

# --- GUI TARGET ---
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(tlou2_disasm_gui
        MANUAL_FINALIZATION
        ${GUI_MAIN}
        ${ALL_SOURCE_FILES}
    )
else()
    add_executable(tlou2_disasm_gui
        ${GUI_MAIN}
        ${ALL_SOURCE_FILES}
    )
endif()

target_link_libraries(tlou2_disasm_gui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets stdc++fs)

target_compile_options(tlou2_disasm_gui PRIVATE
    $<$<CONFIG:Release>:-Ofast -march=znver4>
    $<$<CONFIG:RelWithDebInfo>:-O2 -g -DNDEBUG>
    $<$<CONFIG:Debug>:-O0 -g>
)

set_target_properties(tlou2_disasm_gui PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

# Optional bundle ID
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set_target_properties(tlou2_disasm_gui PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER com.example.tlou2_disasm
    )
endif()

install(TARGETS tlou2_disasm_gui
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(tlou2_disasm_gui)
endif()

# --- CLI TARGET ---
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(tlou2_disasm_cli
        MANUAL_FINALIZATION
        ${CLI_MAIN}
        ${ALL_SOURCE_FILES}
    )
else()
    add_executable(tlou2_disasm_cli
        ${CLI_MAIN}
        ${ALL_SOURCE_FILES}
    )
endif()

target_link_libraries(tlou2_disasm_cli PRIVATE Qt${QT_VERSION_MAJOR}::Widgets stdc++fs)

target_link_options(tlou2_disasm_cli PRIVATE -mconsole)

target_compile_options(tlou2_disasm_cli PRIVATE
    $<$<CONFIG:Release>:-Ofast -march=znver4>
    $<$<CONFIG:RelWithDebInfo>:-O2 -g -DNDEBUG>
    $<$<CONFIG:Debug>:-O0 -g>
)

set_target_properties(tlou2_disasm_cli PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

# Optional bundle ID
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set_target_properties(tlou2_disasm_cli PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER com.example.tlou2_disasm
    )
endif()

install(TARGETS tlou2_disasm_cli
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(tlou2_disasm_cli)
endif()