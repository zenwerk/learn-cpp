add_executable(example_exe)

target_sources(example_exe PUBLIC main.cpp)

target_include_directories(example_exe
    PRIVATE
    ${sfml_SOURCE_DIR}/include
    )

target_link_libraries(example_exe
    PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    )

if(WIN32)
    if (BUILD_SHARED_LIBS)
        add_custom_command(TARGET example_exe POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:sfml-graphics>
            $<TARGET_FILE:sfml-window>
            $<TARGET_FILE:sfml-system>
            $<TARGET_FILE_DIR:example_exe>)
    endif()
endif()