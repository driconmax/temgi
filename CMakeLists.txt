cmake_minimum_required(VERSION 3.20)

project(temgi VERSION 0.1.0 LANGUAGES CXX)

# Selects which of the two console profiles this build (and all its tools) targets.
# OFF (default): 8-bit indexed console, 1 byte/pixel, low memory.
# ON: truecolor console, 2 bytes/pixel (RGB565), a bit more memory per pixel.
option(TEMGI_TRUECOLOR "Build temgi for the truecolor (RGB565) console instead of the 8-bit indexed console" OFF)

if(TEMGI_TRUECOLOR)
    set(TEMGI_PIXEL_RGB565 ON)
endif()

# Generated per build directory (not into include/) so building both console profiles
# side by side (e.g. build/ and build-truecolor/) never clobbers each other's header.
set(TEMGI_GENERATED_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated_include)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/temgi/PixelFormat.h.in
    ${TEMGI_GENERATED_INCLUDE_DIR}/temgi/PixelFormat.h
    @ONLY
)

add_library(temgi STATIC
    src/Console.cpp
    src/Memory.cpp
    src/MemoryManager.cpp
    src/MemoryRegion.cpp
    src/Graphics.cpp
    src/GraphicsProcessor.cpp
    src/BitmapFont.cpp
    src/Input.cpp
    src/Controller.cpp
    src/CartridgeLoader.cpp
)

target_include_directories(temgi
    PUBLIC
        ${TEMGI_GENERATED_INCLUDE_DIR}
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