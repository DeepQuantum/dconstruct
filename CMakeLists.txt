cmake_minimum_required(VERSION 3.16)

project(dconstruct LANGUAGES CXX)

set(VERSION_NUMBER "1.52.0")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source")
add_subdirectory(${SOURCE_DIR}/llvm/llvm)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${SOURCE_DIR}/Graphviz-13.1.0-win64/include"
    "${LLVM_BINARY_DIR}/include"
    "${LLVM_SOURCE_DIR}/include"
)

file(GLOB_RECURSE BASE_SOURCE_FILES
    "${SOURCE_DIR}/disassembly/*.cpp"
    "${SOURCE_DIR}/decompilation/*.cpp"
    "${SOURCE_DIR}/compilation/*.cpp"
    "${SOURCE_DIR}/ast/*.cpp"
    "${SOURCE_DIR}/base/*.cpp"
)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/v1.17.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP true
)

#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

set(LLVM_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(LLVM_ENABLE_PROJECTS "" CACHE STRING "" FORCE)
set(LLVM_TARGETS_TO_BUILD "X86" CACHE STRING "" FORCE)
set(LLVM_EXPERIMENTAL_TARGETS_TO_BUILD "DirectX" CACHE STRING "" FORCE)
set(LLVM_ENABLE_ASSERTIONS OFF CACHE BOOL "" FORCE)


llvm_map_components_to_libnames(LLVM_LIBS
    core
    support
    irreader
    bitreader
    object
    binaryformat
    ipo
    analysis
    passes
)


add_library(dxbc_shader_decomp STATIC 
    "${SOURCE_DIR}/shaders/ndshader.cpp"
)

target_include_directories(dxbc_shader_decomp PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include" 
    "${LLVM_BINARY_DIR}/include"
    "${LLVM_SOURCE_DIR}/include"
)

# Set optimization flags per configuration
set(OPT_FLAGS_RELEASE "-O3")
set(OPT_FLAGS_RELEASE_PROFILE "-O2 -fprofile-use -march=znver4 -mavx512f -fopenmp")
set(OPT_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG -fopenmp")
set(OPT_FLAGS_DEBUG "-O0 -g3")
set(OPT_FLAGS_CREATE_PROFILE "-O2 -fprofile-generate -ftest-coverage -march=znver4 -mavx512f -fopenmp")

# MSVC-specific flags
set(MSVC_FLAGS "/MP")


# Apply flags based on compiler
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
else()
    message("Not applying MSVC")
endif()
target_link_libraries(dxbc_shader_decomp 
    ${LLVM_LIBS}
)

add_library(dconstruct ${BASE_SOURCE_FILES})

if(WIN32)
    target_link_libraries(
        dconstruct
        ${LLVM_LIBS}
        ${SOURCE_DIR}/Graphviz-13.1.0-win64/lib/gvc.lib
        ${SOURCE_DIR}/Graphviz-13.1.0-win64/lib/cgraph.lib
        ${SOURCE_DIR}/Graphviz-13.1.0-win64/lib/gvplugin_core.lib
        ${SOURCE_DIR}/Graphviz-13.1.0-win64/lib/gvplugin_pango.lib
    )
    target_include_directories(dconstruct PRIVATE
        ${SOURCE_DIR}/Graphviz-13.1.0-win64/include
    )
else()
    target_link_libraries(
        dconstruct
        gvc
        cgraph
    )
endif()

target_link_libraries(dconstruct
    $<$<CXX_COMPILER_ID:GNU>: tbb12>
    $<$<CONFIG:CreateProfile>: gcov>
    dxbc_shader_decomp
)

# add_custom_command(
# TARGET dconstruct POST_BUILD
# COMMAND ${CMAKE_COMMAND} -E copy_directory
# " ${CMAKE_SOURCE_DIR}/Graphviz-13.1.0-win64/bin "
# ${CMAKE_CURRENT_BINARY_DIR}
# )


set(CMAKE_CURRENT_TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test")

enable_testing()

add_executable(
    dconstruct_test
    ${CMAKE_CURRENT_TEST_DIR}/disassembler_test.cpp
    ${CMAKE_CURRENT_TEST_DIR}/compiler_test.cpp
    ${CMAKE_CURRENT_TEST_DIR}/decompiler_test.cpp
)

if(MSVC)
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
else()
    message("Not applying MSVC")
endif()

target_link_libraries(dconstruct_test PRIVATE dconstruct GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(dconstruct_test)

add_executable(
    dconstruct_cli
    ${SOURCE_DIR}/main_cli.cpp
)

if(MSVC)
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
else()
    message("Not applying MSVC")
endif()



target_link_libraries(dconstruct_cli PRIVATE dconstruct)

set(BUILDINFO_H "${CMAKE_CURRENT_BINARY_DIR}/buildinfo.h")
add_custom_command(
    OUTPUT ${BUILDINFO_H}
    COMMAND python ${PROJECT_SOURCE_DIR}/buildinfo.py "${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION};${VERSION_NUMBER}"
    DEPENDS
    ${BASE_SOURCE_FILES}
    "${SOURCE_DIR}/main_cli.cpp"
    COMMENT "Generating build info header by running buildinfo.py with compiler version"
)


target_compile_options(dconstruct PRIVATE 
    $<$<CONFIG:Release>:/GL>
)

target_link_options(dconstruct PRIVATE
    $<$<CONFIG:Release>:/LTCG>
)

target_compile_options(dconstruct_cli PRIVATE 
    $<$<CONFIG:Release>:/GL>
)

target_link_options(dconstruct_cli PRIVATE
    $<$<CONFIG:Release>:/LTCG>
)


add_custom_target(generate_buildinfo ALL DEPENDS ${BUILDINFO_H})

add_dependencies(dconstruct_cli generate_buildinfo)

