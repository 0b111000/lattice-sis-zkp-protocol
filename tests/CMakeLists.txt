add_executable(test_protocol
    main_test.cpp
    basic_tests.cpp
    performance_tests.cpp
)

target_link_libraries(test_protocol
    PRIVATE
        lattice_zkp
)

# Enable testing
enable_testing()
add_test(NAME test_protocol COMMAND test_protocol)