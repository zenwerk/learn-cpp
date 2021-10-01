set(CMAKE_CXX_STANDARD 14)

CPMAddPackage(
    NAME sfml
    GITHUB_REPOSITORY SFML/SFML
    GIT_TAG 2.5.1
)

add_executable(sfml_sample
    main.cpp
    )

target_include_directories(sfml_sample
    PRIVATE
    ${sfml_SOURCE_DIR}/include
    )

target_link_libraries(sfml_sample
    PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    )

if (WIN32)
    if (BUILD_SHARED_LIBS)
        add_custom_command(TARGET sfml_sample POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:sfml-graphics>
            $<TARGET_FILE:sfml-window>
            $<TARGET_FILE:sfml-system>
            $<TARGET_FILE_DIR:sfml_sample>)
    endif ()
endif ()