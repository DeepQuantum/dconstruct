cmake_minimum_required(VERSION 3.16)

project(tlou2_disasm VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source")

file(GLOB BASE_SOURCE_FILES
    "${SOURCE_DIR}/disassembly/*.cpp"
    "${SOURCE_DIR}/disassembly/*.h"
)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS "${SOURCE_DIR}/ui")

find_package(QT NAMES Qt6 COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets)

file(GLOB UI_SOURCE_FILES
    ${BASE_SOURCE_FILES}
    "${SOURCE_DIR}/main_gui.cpp"
    "${SOURCE_DIR}/mainwindow.cpp"
    "${SOURCE_DIR}/mainwindow.h"
    "${SOURCE_DIR}/cfg/*.cpp"
    "${SOURCE_DIR}/cfg/*.h"
    "${SOURCE_DIR}/ui/*.ui"
)

# --- GUI TARGET ---
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(tlou2_disasm_gui
        MANUAL_FINALIZATION
        ${UI_SOURCE_FILES}
    )
else()
    add_executable(tlou2_disasm_gui
        ${GUI_MAIN}
        ${UI_SOURCE_FILES}
    )
endif()

target_link_libraries(tlou2_disasm_gui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets stdc++fs)

target_compile_options(tlou2_disasm_gui PRIVATE
    $<$<CONFIG:Release>:-Ofast>
    $<$<CONFIG:Debug>:-Og -g -Wall -Wextra>
    $<$<CONFIG:Profile>:-Ofast -g -Wall -Wextra -pg>
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
add_executable(tlou2_disasm_cli ${BASE_SOURCE_FILES} "${SOURCE_DIR}/main_cli.cpp")

target_compile_options(tlou2_disasm_cli PRIVATE
    $<$<CONFIG:Release>:-O2 -march=znver4 -mavx512f -fopenmp>
    $<$<CONFIG:ReleaseProfile>:-O2 -fprofile-use -march=znver4 -mavx512f -fopenmp>
    $<$<CONFIG:RelWithDebInfo>:-O2 -g -DNDEBUG -fopenmp>
    $<$<CONFIG:Debug>:-O0 -g -fopenmp>
    $<$<CONFIG:CreateProfile>:-O2 -fprofile-generate -ftest-coverage -march=znver4 -mavx512f -fopenmp>
)

target_link_libraries(tlou2_disasm_cli PRIVATE
    stdc++fs
    $<$<CONFIG:CreateProfile>: gcov>
)