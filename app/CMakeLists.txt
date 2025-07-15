add_executable(cpr_example
    main.cpp
)

target_link_libraries(cpr_example PRIVATE
    api
    core
)