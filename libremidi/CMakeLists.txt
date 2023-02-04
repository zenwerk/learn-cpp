set(CMAKE_CXX_STANDARD 17)


add_executable(libremidi-sample
    main.cc
    )

target_include_directories(libremidi-sample
    PRIVATE
    ${libremidi_SOURCE_DIR}
    )

target_link_libraries(libremidi-sample
    PUBLIC
    libremidi
    )
