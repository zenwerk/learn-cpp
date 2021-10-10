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