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
    GIT_TAG 6.0.0
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
    GIT_TAG v2.12.1_libuv_v1.44
    OPTIONS "BUILD_UVW_LIBS YES"
    )

CPMAddPackage(
    NAME mapbox_variant
    GITHUB_REPOSITORY mapbox/variant
    GIT_TAG f87fcbda9daf13fba47a6a889696b0ad23fc098d
    DOWNLOAD_ONLY YES
    )

CPMAddPackage(
    NAME libremidi
    GITHUB_REPOSITORY jcelerier/libremidi
    GIT_TAG a170e09cfc354ea6d1db347f1b52a0e71cd0adbb
    OPTIONS "LIBREMIDI_HEADER_ONLY YES"
    )

add_subdirectory(sfml)

add_subdirectory(rtmidi)

add_subdirectory(libuv)

add_subdirectory(uvw)

add_subdirectory(caper)

add_subdirectory(std)

add_subdirectory(template)

add_subdirectory(thread)

add_subdirectory(libremidi)

add_subdirectory(boost)

add_subdirectory(cpp-rotor)

add_subdirectory(pattern_match)

add_subdirectory(blueprint)

add_subdirectory(push-parser)

add_subdirectory(chatgpt)