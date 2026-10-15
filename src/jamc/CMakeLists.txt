add_subdirectory(core)

add_executable(jamc
    main.cpp
)

target_link_libraries(jamc PRIVATE jamc-core)
