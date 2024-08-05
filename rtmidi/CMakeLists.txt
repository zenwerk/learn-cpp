set(CMAKE_CXX_STANDARD 20)

add_executable(rtmidi_sample
    basic.cc
    )

target_include_directories(rtmidi_sample
    PRIVATE
    ${rtmidi_SOURCE_DIR}
    )

target_link_libraries(rtmidi_sample
    PRIVATE
    rtmidi
    )

if(WIN32)
    if (BUILD_SHARED_LIBS)
        add_custom_command(TARGET rtmidi_sample POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:rtmidi>
            $<TARGET_FILE_DIR:rtmidi_sample>)
    endif()
endif()
