cmake_minimum_required(VERSION 3.20)

project(temgi VERSION 0.1.0 LANGUAGES CXX)

add_library(temgi STATIC
    src/MemoryManager.cpp
    src/MemoryRegion.cpp
    src/Memory.cpp
    src/GraphicsProcessor.cpp
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


#add_custom_command(
#    TARGET game
#    POST_BUILD
#
#    COMMAND temgi-pack
#        $<TARGET_FILE:game>
#        ${CMAKE_CURRENT_SOURCE_DIR}/assets
#        ${CMAKE_CURRENT_BINARY_DIR}/game.temgi
#
#    COMMENT "Building TEMGI cartridge..."
#)