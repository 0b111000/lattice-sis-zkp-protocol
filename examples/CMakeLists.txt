# Simple example
add_executable(simple_example
    simple_example.cpp
)

target_link_libraries(simple_example
    PRIVATE
        lattice_zkp
)
