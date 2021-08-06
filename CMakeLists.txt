cmake_minimum_required(VERSION 3.16)
project(awesomeProjectCPP14
    LANGUAGES CXX
    VERSION 1.0
    )

set(CMAKE_CXX_STANDARD 14)

include(cmake/tools.cmake)

add_compile_options(
    -Wall
)

CPMAddPackage(
    NAME sfml
    GITHUB_REPOSITORY SFML/SFML
    GIT_TAG 2.5.1
)

add_subdirectory(src)