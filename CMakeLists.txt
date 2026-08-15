cmake_minimum_required(VERSION 3.20)

project(temgi-console LANGUAGES CXX)

add_library(temgi-console STATIC
    src/Console.cpp
    src/Graphics.cpp
    src/Input.cpp
    src/Memory.cpp
    src/MemoryManager.cpp
    src/MemoryRegion.cpp
    src/GraphicsProcessor.cpp
    src/CartridgeLoader.cpp
)

target_include_directories(temgi-console
    PUBLIC include
)

target_compile_features(temgi-console
    PUBLIC cxx_std_20
)

add_library(TEMGI::Console ALIAS temgi-console)