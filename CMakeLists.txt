cmake_minimum_required(VERSION 3.0)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# improve CMAKE_CXX_FLAGS_RELEASE to every possible optimization considering Eigen3, target arch (my CPU is a Ryzen 5950X), and compiler
# also linux kernel features and whatnot
# aside from "-O3"
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -mfma -ffast-math -fno-signed-zeros -fno-trapping-math -fassociative-math -freciprocal-math -fno-math-errno -fno-rounding-math -fno-signaling-nans -fno-unsafe-math-optimizations -fno-trapping-math -fno-math-errno")

project(pitchlite)
enable_testing()

# set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# include src/ as include directory
include_directories(src)

# include src/*.cpp and src/*.c as source files
file(GLOB SOURCES "src/*.cpp")

# compile vendored submodule kissfft
set(KISSFFT_TEST OFF CACHE BOOL "Disable kissfft tests")
set(KISSFFT_TOOLS OFF CACHE BOOL "Disable kissfft tools")
set(KISSFFT_STATIC TRUE CACHE BOOL "Compile kissfft as static library")
add_subdirectory(vendor/kissfft)
include_directories(vendor/kissfft)
include_directories(${CMAKE_BINARY_DIR}/vendor/kissfft)

# Create the Emscripten target
if(EMSCRIPTEN)
    # compile library
    add_executable(pitchlite ${SOURCES})

    target_link_libraries(pitchlite kissfft)

    set_target_properties(pitchlite PROPERTIES
        LINK_FLAGS "-s WASM=1 -s ASSERTIONS=1 -s ALLOW_MEMORY_GROWTH=1 -s MODULARIZE=1 -s EXPORT_NAME=pitchlite -s EXPORTED_FUNCTIONS=\"['_malloc', '_free', '_memset', '_pitchliteInit', '_pitchlitePitches', '_pitchliteDeinit']\""
    )

    # copy built files to ${CMAKE_SOURCE_DIR}/web
    add_custom_command(TARGET pitchlite POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/pitchlite.js ${CMAKE_SOURCE_DIR}/web
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/pitchlite.wasm ${CMAKE_SOURCE_DIR}/web
    )
# if not emscripten, lint targets only, etc.
else()
    # remove wasm glue file
    foreach(source ${SOURCES})
        get_filename_component(filename ${source} NAME)
        if(filename MATCHES "wasm_glue.cpp$")
            list(REMOVE_ITEM SOURCES ${source})
        endif()
    endforeach()

    # add library
    add_library(pitchlite STATIC ${SOURCES})

    # link to kissfft
    target_link_libraries(pitchlite kissfft)

    # add tests with gtest and gtest main
    # include test/*.cpp as test files
    file(GLOB TEST_SOURCES "test/*.cpp")

    add_executable(pitchlite.tests ${TEST_SOURCES})
    target_link_libraries(pitchlite.tests pitchlite gtest gtest_main)
    add_test(NAME tests COMMAND pitchlite.test)

    file(GLOB SOURCES_TO_LINT "src/*.cpp" "src/*.hpp" "test/*.cpp" "test/*.hpp")

    # add target to run standard lints and formatters
    add_custom_target(lint
        COMMAND clang-format -i ${SOURCES_TO_LINT} --style=file
        # add clang-tidy command
        # add include dirs to clang-tidy
        COMMAND cppcheck --enable=all --suppress=missingIncludeSystem ${SOURCES_TO_LINT} --std=c++17
        COMMAND scan-build -o ${CMAKE_BINARY_DIR}/scan-build-report make -C ${CMAKE_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()
