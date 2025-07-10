cmake_minimum_required(VERSION 3.16)

project(dconstruct LANGUAGES CXX)

set(VERSION_NUMBER "beta_1")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source")

file(GLOB BASE_SOURCE_FILES
    "${SOURCE_DIR}/disassembly/*.cpp"
    "${SOURCE_DIR}/disassembly/*.h"
)

# set(CMAKE_AUTOUIC ON)
# set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTORCC ON)

# set(CMAKE_AUTOUIC_SEARCH_PATHS "${SOURCE_DIR}/ui")

# find_package(QT NAMES Qt6 COMPONENTS Widgets)
# find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets)

# file(GLOB UI_SOURCE_FILES
# ${BASE_SOURCE_FILES}
# "${SOURCE_DIR}/main_gui.cpp"
# "${SOURCE_DIR}/mainwindow.cpp"
# "${SOURCE_DIR}/mainwindow.h"
# "${SOURCE_DIR}/cfg/*.cpp"
# "${SOURCE_DIR}/cfg/*.h"
# "${SOURCE_DIR}/ui/*.ui"
# )

# # --- GUI TARGET ---
# if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
# qt_add_executable(tlou2_disasm_gui
# MANUAL_FINALIZATION
# ${UI_SOURCE_FILES}
# )
# else()
# add_executable(tlou2_disasm_gui
# ${GUI_MAIN}
# ${UI_SOURCE_FILES}
# )
# endif()

# target_link_libraries(tlou2_disasm_gui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets stdc++fs)

# target_compile_options(tlou2_disasm_gui PRIVATE
# $<$<CONFIG:Release>:-Ofast>
# $<$<CONFIG:Debug>:-Og -g -Wall -Wextra>
# $<$<CONFIG:Profile>:-Ofast -g -Wall -Wextra -pg>
# )

# set_target_properties(tlou2_disasm_gui PROPERTIES
# MACOSX_BUNDLE TRUE
# WIN32_EXECUTABLE TRUE
# MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
# MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
# )

# # Optional bundle ID
# if(${QT_VERSION} VERSION_LESS 6.1.0)
# set_target_properties(tlou2_disasm_gui PROPERTIES
# MACOSX_BUNDLE_GUI_IDENTIFIER com.example.tlou2_disasm
# )
# endif()

# install(TARGETS tlou2_disasm_gui
# BUNDLE DESTINATION .
# LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
# RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
# )

# if(QT_VERSION_MAJOR EQUAL 6)
# qt_finalize_executable(tlou2_disasm_gui)
# endif()

# --- CLI TARGET ---
add_executable(dconstruct ${BASE_SOURCE_FILES} "${SOURCE_DIR}/main_cli.cpp")

target_compile_options(dconstruct PRIVATE
    $<$<CONFIG:Release>:-O3 -flto -Wall -g0>
    $<$<CONFIG:ReleaseProfile>:-O2 -fprofile-use -march=znver4 -mavx512f -fopenmp>
    $<$<CONFIG:RelWithDebInfo>:-O2 -g -DNDEBUG -fopenmp>
    $<$<CONFIG:Debug>:-O0 -g3 -flto -Wall>
    $<$<CONFIG:CreateProfile>:-O2 -fprofile-generate -ftest-coverage -march=znver4 -mavx512f -fopenmp>
)
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${SOURCE_DIR}/Graphviz-13.1.0-win64/include
)
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${SOURCE_DIR}/Graphviz-13.1.0-win64/include
)
target_compile_definitions(dconstruct PRIVATE
    CXXOPTS_NO_EXCEPTIONS=1
)

link_directories("${SOURCE_DIR}/Graphviz-13.1.0-win64/lib")

target_link_libraries(dconstruct PRIVATE
    ${SOURCE_DIR}/Graphviz-13.1.0-win64/lib/gvc.lib
    ${SOURCE_DIR}/Graphviz-13.1.0-win64/lib/cgraph.lib
    ${SOURCE_DIR}/Graphviz-13.1.0-win64/lib/gvplugin_core.lib
    ${SOURCE_DIR}/Graphviz-13.1.0-win64/lib/gvplugin_pango.lib
    $<$<CXX_COMPILER_ID:GNU>: tbb12>
    $<$<CONFIG:CreateProfile>: gcov>
)

# add_custom_command(
# TARGET dconstruct POST_BUILD
# COMMAND ${CMAKE_COMMAND} -E copy_directory
# "${CMAKE_SOURCE_DIR}/Graphviz-13.1.0-win64/bin"
# ${CMAKE_CURRENT_BINARY_DIR}
# )
set(BUILDINFO_H "${CMAKE_CURRENT_BINARY_DIR}/buildinfo.h")
add_custom_command(
    OUTPUT ${BUILDINFO_H}
    COMMAND python ${PROJECT_SOURCE_DIR}/buildinfo.py "${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION};${VERSION_NUMBER}"
    DEPENDS
    ${BASE_SOURCE_FILES}
    "${SOURCE_DIR}/main_cli.cpp"
    COMMENT "Generating build info header by running buildinfo.py with compiler version"
)

add_custom_target(generate_buildinfo ALL DEPENDS ${BUILDINFO_H})

add_dependencies(dconstruct generate_buildinfo)
