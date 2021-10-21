set(CMAKE_CXX_STANDARD 17)


include_directories(
    ${uvw_SOURCE_DIR}/src
    ${libuv_SOURCE_DIR}/include
    )

link_libraries(uvw-static)

add_executable(uvw-tcp-sample
    tcp-sample.cc
    )

add_executable(uvw-idle-basic
    idle-basic.cc
    )

add_executable(uvw-idle-rtmidi
    idle-rtmidi.cc
    )

target_include_directories(uvw-idle-rtmidi
    PRIVATE
    ${rtmidi_SOURCE_DIR}
    )

target_link_libraries(uvw-idle-rtmidi
    PRIVATE
    rtmidi
    )

add_executable(uvw-async-basic
    async-basic.cc)

add_executable(uvw-tty-basic
    tty-basic.cc)

add_executable(uvw-queue-work
    queue-work.cc)

add_executable(uvw-timer-basic
    timer-basic.cc)