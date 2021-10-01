# 依存関係のインストール
CPMAddPackage(
    NAME rtmidi
    GITHUB_REPOSITORY thestk/rtmidi
    GIT_TAG 4.0.0
)

add_executable(rtmidi_sample
    main.cc
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
