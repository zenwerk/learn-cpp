set(CMAKE_CXX_STANDARD 20)

include_directories(
    ${rtmidi_SOURCE_DIR}
)

link_libraries(rtmidi)


add_executable(rtmidi_sample
    basic.cc
    )

add_executable(rtmidi_beat_example
    beat.cc
    )

add_executable(rtmidi_beat_example_2
    beat2.cc
    )

if(WIN32)
    if (BUILD_SHARED_LIBS)
        add_custom_command(TARGET rtmidi_sample POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:rtmidi>
            $<TARGET_FILE_DIR:rtmidi_sample>)
    endif()
endif()
