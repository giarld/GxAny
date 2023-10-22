cmake_minimum_required(VERSION 3.20)

# test_gx
add_executable(TestGAny
        src/test_main.cpp

        src/test_gany.cpp
        src/test_enum.cpp
        src/test_reflection.cpp
)

target_link_libraries(TestGAny gtest gany-core)
