cmake_minimum_required(VERSION 3.16)

project(dconstruct LANGUAGES CXX)

set(VERSION_NUMBER "2.0.0")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source")
# add_subdirectory(${SOURCE_DIR}/llvm/llvm)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${SOURCE_DIR}/Graphviz-13.1.0-win64/include"
    "${SOURCE_DIR}/qui/source/ctre/single-header"
    # "${LLVM_BINARY_DIR}/include"
    # "${LLVM_SOURCE_DIR}/include"
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

# set(LLVM_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
# set(LLVM_ENABLE_PROJECTS "" CACHE STRING "" FORCE)
# set(LLVM_TARGETS_TO_BUILD "X86" CACHE STRING "" FORCE)
# set(LLVM_EXPERIMENTAL_TARGETS_TO_BUILD "DirectX" CACHE STRING "" FORCE)
# set(LLVM_ENABLE_ASSERTIONS OFF CACHE BOOL "" FORCE)


# llvm_map_components_to_libnames(LLVM_LIBS
#     core
#     support
#     irreader
#     bitreader
#     object
#     binaryformat
#     ipo
#     analysis
#     passes
# )


# add_library(dxbc_shader_decomp STATIC 
#     "${SOURCE_DIR}/shaders/ndshader.cpp"
# )

# target_include_directories(dxbc_shader_decomp PRIVATE
#     "${CMAKE_CURRENT_SOURCE_DIR}/include" 
#     "${LLVM_BINARY_DIR}/include"
#     "${LLVM_SOURCE_DIR}/include"
# )

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
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/wd4624>)
else()
    message("Not applying MSVC")
endif()
# target_link_libraries(dxbc_shader_decomp 
#     ${LLVM_LIBS}
# )

add_library(dconstruct_lib ${BASE_SOURCE_FILES})

if(WIN32)
    target_link_libraries(
        dconstruct_lib
        # ${LLVM_LIBS}
        ${SOURCE_DIR}/Graphviz-13.1.0-win64/lib/gvc.lib
        ${SOURCE_DIR}/Graphviz-13.1.0-win64/lib/cgraph.lib
        ${SOURCE_DIR}/Graphviz-13.1.0-win64/lib/gvplugin_core.lib
        ${SOURCE_DIR}/Graphviz-13.1.0-win64/lib/gvplugin_pango.lib
    )
    target_include_directories(dconstruct_lib PRIVATE
        ${SOURCE_DIR}/Graphviz-13.1.0-win64/include
    )
else()
    target_link_libraries(
        dconstruct_lib
        gvc
        cgraph
    )
endif()

target_link_libraries(dconstruct_lib
    $<$<CXX_COMPILER_ID:GNU>: tbb12>
    $<$<CONFIG:CreateProfile>: gcov>
    # dxbc_shader_decomp
)

if(WIN32)
    set(GRAPHVIZ_BIN "${SOURCE_DIR}/Graphviz-13.1.0-win64/bin")
    set(DCONSTRUCT_RUNTIME_DLLS
        "${GRAPHVIZ_BIN}/cdt.dll"
        "${GRAPHVIZ_BIN}/cgraph.dll"
        "${GRAPHVIZ_BIN}/gvc.dll"
        "${GRAPHVIZ_BIN}/gvplugin_core.dll"
        "${GRAPHVIZ_BIN}/gvplugin_dot_layout.dll"
        "${GRAPHVIZ_BIN}/gvplugin_pango.dll"
        "${GRAPHVIZ_BIN}/libexpat.dll"
        "${GRAPHVIZ_BIN}/pathplan.dll"
        "${GRAPHVIZ_BIN}/xdot.dll"
    )
    add_custom_command(TARGET dconstruct_lib POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DCONSTRUCT_RUNTIME_DLLS} "$<TARGET_FILE_DIR:dconstruct_lib>"
        COMMENT "Copying runtime DLLs next to dconstruct_lib"
        VERBATIM
    )
endif()


set(CMAKE_CURRENT_TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test")

enable_testing()

add_executable(
    dconstruct_test
    ${CMAKE_CURRENT_TEST_DIR}/cpp/disassembler_test.cpp
    ${CMAKE_CURRENT_TEST_DIR}/cpp/compiler_test.cpp
    ${CMAKE_CURRENT_TEST_DIR}/cpp/decompiler_test.cpp
    ${CMAKE_CURRENT_TEST_DIR}/cpp/binaryfile_test.cpp
)

if(MSVC)
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
else()
    message("Not applying MSVC")
endif()

target_link_libraries(dconstruct_test PRIVATE dconstruct_lib GTest::gtest_main)
target_compile_definitions(dconstruct_test PRIVATE DCONSTRUCT_TEST_ROOT="${CMAKE_CURRENT_TEST_DIR}")

include(GoogleTest)
gtest_discover_tests(dconstruct_test WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(
    dconstruct
    ${SOURCE_DIR}/main_cli.cpp
)

if(MSVC)
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
else()
    message("Not applying MSVC")
endif()



target_link_libraries(dconstruct PRIVATE dconstruct_lib)


set(BUILDINFO_H "${CMAKE_CURRENT_BINARY_DIR}/buildinfo.h")
add_custom_command(
    OUTPUT ${BUILDINFO_H}
    COMMAND python ${PROJECT_SOURCE_DIR}/scripts/buildinfo.py "${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION};${VERSION_NUMBER}"
    DEPENDS
    ${BASE_SOURCE_FILES}
    "${SOURCE_DIR}/main_cli.cpp"
    COMMENT "Generating build info header by running scripts/buildinfo.py with compiler version"
)


target_compile_options(dconstruct_lib PRIVATE
    $<$<CONFIG:Release>:/GL>
    $<$<CXX_COMPILER_ID:GNU,Clang>:-ftemplate-depth=4096>
    $<$<CXX_COMPILER_ID:MSVC>:/templateDepth:8192>
)

target_link_options(dconstruct_lib PRIVATE
    $<$<CONFIG:Release>:/LTCG>
)

target_compile_options(dconstruct PRIVATE 
    $<$<CONFIG:Release>:/GL>
)

target_link_options(dconstruct PRIVATE
    $<$<CONFIG:Release>:/LTCG>
)


add_custom_target(generate_buildinfo ALL DEPENDS ${BUILDINFO_H})

add_dependencies(dconstruct generate_buildinfo)

add_executable(
    dcc
    ${SOURCE_DIR}/main_dcc.cpp
)


target_link_libraries(dcc PRIVATE dconstruct_lib)

add_executable(
    decode_sidbase
    ${SOURCE_DIR}/sidbase/tools/decode_sidbase.cpp
)

set(BUILDINFO_H "${CMAKE_CURRENT_BINARY_DIR}/buildinfo.h")
add_custom_command(
    OUTPUT ${BUILDINFO_H}
    COMMAND python ${PROJECT_SOURCE_DIR}/scripts/buildinfo.py "${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION};${VERSION_NUMBER}"
    DEPENDS
    ${BASE_SOURCE_FILES}
    "${SOURCE_DIR}/main_cli.cpp"
    COMMENT "Generating build info header by running scripts/buildinfo.py with compiler version"
)

target_compile_options(dcc PRIVATE 
    $<$<CONFIG:Release>:/GL>
)

target_link_options(dcc PRIVATE
    $<$<CONFIG:Release>:/LTCG>
)

add_dependencies(dconstruct generate_buildinfo)


add_subdirectory(${SOURCE_DIR}/minhook)

add_subdirectory(${SOURCE_DIR}/qui)

add_executable(dconstruct_ui
    ${SOURCE_DIR}/ui/main.cpp
    $<$<BOOL:${WIN32}>:${CMAKE_SOURCE_DIR}/resources/dconstruct.rc>
)

target_link_libraries(dconstruct_ui PRIVATE
    qui_base::qui_base
    dconstruct_lib
)

target_include_directories(dconstruct_ui PRIVATE ${SOURCE_DIR}/qui/source/stb)
add_dependencies(dconstruct_ui generate_buildinfo)

add_custom_command(TARGET dconstruct_ui POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:dconstruct_ui>/fonts"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/resources/GeistMono-Medium.ttf"
        "${CMAKE_SOURCE_DIR}/resources/GeistMono-SemiBold.ttf"
        "${CMAKE_SOURCE_DIR}/resources/GeistMono-Bold.ttf"
        "${CMAKE_SOURCE_DIR}/resources/OFL.txt"
        "$<TARGET_FILE_DIR:dconstruct_ui>/fonts"
    COMMENT "Copying fonts and license into $<TARGET_FILE_DIR:dconstruct_ui>/fonts"
    VERBATIM
)

if(WIN32)
    target_link_libraries(dconstruct_ui PRIVATE comdlg32 ole32 shell32)
    target_include_directories(dconstruct_ui PRIVATE ${CMAKE_SOURCE_DIR}/resources)
endif()

set_target_properties(dconstruct_ui PROPERTIES
    CXX_STANDARD ${CMAKE_CXX_STANDARD}
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_compile_options(dconstruct_ui PRIVATE
    $<$<CONFIG:Release>:/GL>
)

target_link_options(dconstruct_ui PRIVATE
    $<$<CONFIG:Release>:/LTCG>
    $<$<CXX_COMPILER_ID:MSVC>:/SUBSYSTEM:WINDOWS>
    $<$<CXX_COMPILER_ID:MSVC>:/ENTRY:mainCRTStartup>
)

if(MSVC)
    enable_language(ASM_MASM)
endif()

add_library(dconstruct_hooking SHARED
    "${SOURCE_DIR}/hooking/hooking.cpp"
    "${SOURCE_DIR}/hooking/extended_opcodes.asm"
    "${SOURCE_DIR}/base/sidbase.cpp"
)

target_include_directories(dconstruct_hooking PRIVATE "${SOURCE_DIR}/minhook/include")

target_link_libraries(dconstruct_hooking PRIVATE minhook)

target_compile_options(dconstruct_hooking PRIVATE 
    $<$<AND:$<CONFIG:Release>,$<COMPILE_LANGUAGE:CXX>>:/GL>
)

target_link_options(dconstruct_hooking PRIVATE
    $<$<CONFIG:Release>:/LTCG>
)
