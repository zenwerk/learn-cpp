cmake_minimum_required(VERSION 3.16)
project(awesomeCppProject
    LANGUAGES C CXX
    VERSION 0.1.0
    )

include(cmake/tools.cmake)

add_compile_options(
    -Wall
)

add_subdirectory(sfml)

add_subdirectory(rtmidi)