set(CMAKE_CXX_STANDARD 17)

add_executable(idle-basic)

target_sources(idle-basic
    PUBLIC
    idle-basic.cc
    )

target_include_directories(idle-basic
    PRIVATE
    ${libuv_SOURCE_DIR}/include
    )

target_link_libraries(idle-basic
    PRIVATE
    uv_a
    )

add_executable(idle-basic-2
    idle-basic-2.cc)
target_include_directories(idle-basic-2
    PRIVATE
    ${libuv_SOURCE_DIR}/include
    )

target_link_libraries(idle-basic-2
    PRIVATE
    uv_a
    )


#if(WIN32)
#    if (BUILD_SHARED_LIBS)
#        add_custom_command(TARGET idle-basic POST_BUILD
#            COMMAND ${CMAKE_COMMAND} -E copy_if_different
#            #$<TARGET_FILE:rtmidi>
#            $<TARGET_FILE:uv>
#            $<TARGET_FILE_DIR:idle-basic>)
#    endif()
#endif()
