add_library(jamc-core
    lex.cpp
    cli_parse.cpp
    errors.cpp
)

target_include_directories(jamc-core
    PUBLIC ${CMAKE_SOURCE_DIR}/include
)
