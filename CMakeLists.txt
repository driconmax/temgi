cmake_minimum_required(VERSION 3.20)

project(temgi VERSION 0.1.0 LANGUAGES CXX)

add_library(temgi STATIC
    src/Console.cpp
    src/Memory.cpp
    src/MemoryManager.cpp
    src/MemoryRegion.cpp
    src/Graphics.cpp
    src/GraphicsProcessor.cpp
    src/Input.cpp
    src/CartridgeLoader.cpp
)

target_include_directories(temgi
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(temgi
    PUBLIC cxx_std_20
)

add_library(TEMGI::Console ALIAS temgi)


### TESTS ###

add_executable(memory-test
    tests/MemoryTest.cpp
)

target_link_libraries(memory-test
    PRIVATE temgi
)

target_include_directories(memory-test
    PRIVATE src
)


add_executable(graphics-test
    tests/GraphicsTest.cpp
)

target_link_libraries(graphics-test
    PRIVATE temgi
)

target_include_directories(graphics-test
    PRIVATE src
)


### PACKER ###

add_executable(temgi-pack
    tools/pack/main.cpp
)

target_link_libraries(temgi-pack
    PRIVATE temgi
)


### TIMG - Image ###

add_executable(temgi-image
    tools/image/main.cpp
)

target_include_directories(temgi-image
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb
)

target_link_libraries(temgi-image
    PRIVATE temgi
)


### ATIMG - Image Atlas ###

add_executable(temgi-atlas
    tools/atlas/main.cpp
)

target_include_directories(temgi-atlas
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb
)

target_link_libraries(temgi-atlas
    PRIVATE temgi
)