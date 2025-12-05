cmake_minimum_required(VERSION 3.16)

project(dconstruct LANGUAGES CXX)

set(VERSION_NUMBER "beta_3")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source")

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

add_executable(dconstruct ${BASE_SOURCE_FILES} "${SOURCE_DIR}/main_cli.cpp")

target_compile_options(dconstruct PRIVATE
    $<$<CONFIG:Release>:-O3 -Wall -g0>
    $<$<CONFIG:ReleaseProfile>:-O2 -fprofile-use -march=znver4 -mavx512f -fopenmp>
    $<$<CONFIG:RelWithDebInfo>:-O2 -g -DNDEBUG -fopenmp>
    $<$<CONFIG:Debug>:-O0 -g3 -Wall>
    $<$<CONFIG:CreateProfile>:-O2 -fprofile-generate -ftest-coverage -march=znver4 -mavx512f -fopenmp>
)

add_compile_options($<$<C_COMPILER_ID:MSVC>:/MP>
                    $<$<CXX_COMPILER_ID:MSVC>:/MP>)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_include_directories(dconstruct PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

if(WIN32)
    target_link_libraries(
        dconstruct
        ${SOURCE_DIR}/Graphviz-13.1.0-win64/lib/gvc.lib
        ${SOURCE_DIR}/Graphviz-13.1.0-win64/lib/cgraph.lib
        ${SOURCE_DIR}/Graphviz-13.1.0-win64/lib/gvplugin_core.lib
        ${SOURCE_DIR}/Graphviz-13.1.0-win64/lib/gvplugin_pango.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMCore.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMSupport.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMIRReader.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMBitReader.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMBitWriter.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMAnalysis.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMScalarOpts.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMInstCombine.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMTransformUtils.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMTarget.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMAsmParser.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMAsmPrinter.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMSelectionDAG.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMCodeGen.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMGlobalISel.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMMC.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMMCParser.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMMCDisassembler.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMX86CodeGen.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMX86Desc.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMX86Info.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMX86AsmParser.lib
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

if(WIN32)
    target_link_libraries(
        dconstruct_test
        ${SOURCE_DIR}/Graphviz-13.1.0-win64/lib/gvc.lib
        ${SOURCE_DIR}/Graphviz-13.1.0-win64/lib/cgraph.lib
        ${SOURCE_DIR}/Graphviz-13.1.0-win64/lib/gvplugin_core.lib
        ${SOURCE_DIR}/Graphviz-13.1.0-win64/lib/gvplugin_pango.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMCore.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMSupport.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMIRReader.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMBitReader.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMBitWriter.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMAnalysis.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMScalarOpts.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMInstCombine.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMTransformUtils.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMTarget.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMAsmParser.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMAsmPrinter.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMSelectionDAG.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMCodeGen.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMGlobalISel.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMMC.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMMCParser.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMMCDisassembler.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMX86CodeGen.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMX86Desc.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMX86Info.lib
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib/LLVMX86AsmParser.lib
    )
    target_include_directories(dconstruct_test PRIVATE
        ${SOURCE_DIR}/Graphviz-13.1.0-win64/include
    )
else()
    target_link_libraries(
        dconstruct_test
        gvc
        cgraph
    )
endif()

target_link_libraries(
    dconstruct_test
    $<$<CXX_COMPILER_ID:GNU>: tbb12>
    $<$<CONFIG:CreateProfile>: gcov>
    GTest::gtest_main
)

target_include_directories(dconstruct_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

include(GoogleTest)
gtest_discover_tests(dconstruct_test)
