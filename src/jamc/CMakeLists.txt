add_library(jamc
    lex.cpp
    cli_parse.cpp
    errors.cpp
)

target_include_directories(jamc
    PUBLIC ${CMAKE_SOURCE_DIR}/include
    common-compiler-flags
)

add_executable(jamc-wrap
    main.cpp
)

target_link_libraries(jamc-wrap
    PRIVATE jamc
    common-compiler-flags
)
