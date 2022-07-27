find_program(CAPER NAMES caper caper.exe)
if(NOT CAPER)
    message(WARNING "caper (or caper.exe) program is required for calc")
endif()

find_program(RE2C NAMES re2c re2c.exe)
if(NOT RE2C)
    message(FATAL_ERROR "re2c (or re2c.exe) program is required for calc")
endif()


add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/calc.hpp
    COMMAND ${CAPER} ${CMAKE_CURRENT_SOURCE_DIR}/calc.cpg ${CMAKE_CURRENT_SOURCE_DIR}/calc.hpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/calc.cpg
    COMMENT "Generating calc.hpp"
    #    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/lex.cc
    COMMAND ${RE2C} -i -o ${CMAKE_CURRENT_SOURCE_DIR}/lex.cc ${CMAKE_CURRENT_SOURCE_DIR}/lex.re.cc
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lex.re.cc
    COMMENT "Generating lex.cc"
    #    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(lex.cc ALL
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/lex.cc
    )

add_custom_target(calc.hpp ALL
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/calc.hpp
    )


add_executable(caper-calc
    main.cc)

add_dependencies(caper-calc
    calc.hpp
    lex.cc
    )