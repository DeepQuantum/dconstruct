cmake_minimum_required(VERSION 3.16)

project(dconstruct LANGUAGES CXX)

set(VERSION_NUMBER "beta_3")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source")

file(GLOB BASE_SOURCE_FILES
    "${SOURCE_DIR}/disassembly/*.cpp"
    "${SOURCE_DIR}/disassembly/*.h"
    "${SOURCE_DIR}/decompilation/*.h"
    "${SOURCE_DIR}/decompilation/*.cpp"
    "${SOURCE_DIR}/compilation/*.cpp"
    "${SOURCE_DIR}/compilation/*.h"
)

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/v1.17.0.zip
)

# Prevent GTest from overriding your compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

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
target_compile_definitions(dconstruct PRIVATE
    CXXOPTS_NO_EXCEPTIONS=1
)

# link_directories("${SOURCE_DIR}/Graphviz-13.1.0-win64/lib")
target_link_libraries(dconstruct PRIVATE
    ${SOURCE_DIR}/Graphviz-13.1.0-win64/lib/gvc.lib
    ${SOURCE_DIR}/Graphviz-13.1.0-win64/lib/cgraph.lib
    ${SOURCE_DIR}/Graphviz-13.1.0-win64/lib/gvplugin_core.lib
    ${SOURCE_DIR}/Graphviz-13.1.0-win64/lib/gvplugin_pango.lib
    $<$<CXX_COMPILER_ID:GNU>: tbb12>
    $<$<CONFIG:CreateProfile>: gcov>
)

target_include_directories(dconstruct PRIVATE
    "${SOURCE_DIR}/disassembly"
    "${SOURCE_DIR}/decompilation"
)

# add_custom_command(
# TARGET dconstruct POST_BUILD
# COMMAND ${CMAKE_COMMAND} -E copy_directory
# " ${CMAKE_SOURCE_DIR}/Graphviz-13.1.0-win64/bin "
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

set(CMAKE_CURRENT_TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test")

enable_testing()

add_executable(
    dconstruct_test
    ${BASE_SOURCE_FILES}
    ${CMAKE_CURRENT_TEST_DIR}/disassembler_test.cpp
    ${CMAKE_CURRENT_TEST_DIR}/compiler_test.cpp
    ${CMAKE_CURRENT_TEST_DIR}/decompiler_test.cpp
)

target_link_libraries(
    dconstruct_test
    ${SOURCE_DIR}/Graphviz-13.1.0-win64/lib/gvc.lib
    ${SOURCE_DIR}/Graphviz-13.1.0-win64/lib/cgraph.lib
    ${SOURCE_DIR}/Graphviz-13.1.0-win64/lib/gvplugin_core.lib
    ${SOURCE_DIR}/Graphviz-13.1.0-win64/lib/gvplugin_pango.lib
    $<$<CXX_COMPILER_ID:GNU>: tbb12>
    $<$<CONFIG:CreateProfile>: gcov>
    GTest::gtest_main
)

target_include_directories(dconstruct_test PRIVATE
    "${SOURCE_DIR}/disassembly"
    "${SOURCE_DIR}/decompilation"
    "${SOURCE_DIR}/compilation"
)

include(GoogleTest)
gtest_discover_tests(dconstruct_test)
