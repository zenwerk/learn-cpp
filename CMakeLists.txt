cmake_minimum_required(VERSION 3.16)
project(awesomeProjectCpp
    LANGUAGES C CXX
    VERSION 0.1.0
    )

include(cmake/tools.cmake)

add_compile_options(
    -Wall
)

CPMAddPackage(
    NAME rtmidi
    GITHUB_REPOSITORY thestk/rtmidi
    GIT_TAG 4.0.0
    OPTIONS "RTMIDI_BUILD_STATIC_LIBS YES"
)

CPMAddPackage(
    NAME libuv
    GITHUB_REPOSITORY libuv/libuv
    GIT_TAG v1.42.0
)

CPMAddPackage(
    NAME uvw
    GITHUB_REPOSITORY skypjack/uvw
    GIT_TAG v2.10.0_libuv_v1.42
    OPTIONS "BUILD_UVW_LIBS YES"
)

add_subdirectory(sfml)

add_subdirectory(rtmidi)

add_subdirectory(libuv)

add_subdirectory(uvw)