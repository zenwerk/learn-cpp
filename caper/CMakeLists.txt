set(CMAKE_CXX_STANDARD 17)

find_program(CAPER NAMES caper caper.exe)
if(NOT CAPER)
    message(FATAL_ERROR "caper (or caper.exe) program is required")
endif()

include_directories(
    ${mapbox_variant_SOURCE_DIR}/include
    )

add_executable(caper-hello0
    hello0.cc)

add_executable(caper-hello1
    hello1.cc)

add_executable(caper-hello2
    hello2.cc)

add_executable(caper-hello3
    hello3.cc)

add_executable(caper-hello4
    hello4.cc)

add_executable(caper-hello5
    hello5.cc)

add_executable(caper-hello6
    hello6.cc)

add_executable(caper-hello7-shared-ptr
    hello7_shared_ptr.cc)

add_subdirectory(calc)