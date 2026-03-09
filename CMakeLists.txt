cmake_minimum_required(VERSION 3.16)
project(FileZipper VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

# ── Include directories ──────────────────────────────────────────────────────
include_directories(include)

# ── Shared source files (no main.cpp) ───────────────────────────────────────
set(LIB_SOURCES
    src/huffman/HuffmanTree.cpp
    src/huffman/HuffmanEncoder.cpp
    src/huffman/HuffmanDecoder.cpp
    src/io/BitWriter.cpp
    src/io/BitReader.cpp
    src/utils/FrequencyTable.cpp
)

# ── Main executable ──────────────────────────────────────────────────────────
add_executable(filezipper src/main.cpp ${LIB_SOURCES})
target_include_directories(filezipper PRIVATE include)

# ── Google Test (optional) ───────────────────────────────────────────────────
find_package(GTest QUIET)
if(GTest_FOUND)
    enable_testing()
    add_executable(filezipper_tests tests/huffman_test.cpp ${LIB_SOURCES})
    # FIX: was missing — caused "No such file" compile errors on test target
    target_include_directories(filezipper_tests PRIVATE include)
    target_link_libraries(filezipper_tests GTest::GTest GTest::Main)
    add_test(NAME HuffmanTests COMMAND filezipper_tests)
endif()

install(TARGETS filezipper DESTINATION bin)