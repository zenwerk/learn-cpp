set(CMAKE_CXX_STANDARD 17)


add_executable(libremidi_sample
    main.cc
    )

target_include_directories(libremidi_sample
    PRIVATE
    ${libremidi_SOURCE_DIR}
    )

target_link_libraries(libremidi_sample
    PUBLIC
    libremidi
    )
