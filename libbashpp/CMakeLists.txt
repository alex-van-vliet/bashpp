add_library(libbashpp
    bashpp/command.hpp src/command.cpp
    bashpp/env.hpp src/env.cpp
    bashpp/file_descriptor.hpp src/file_descriptor.cpp
    bashpp/node.hpp src/node.cpp
    bashpp/pipeline.hpp src/pipeline.cpp
    bashpp/process.hpp src/process.cpp
    bashpp/visitor.hpp src/visitor.cpp
    bashpp/wrappers.hpp src/wrappers.cpp
)
target_compile_options(libbashpp PRIVATE -Wall -Wextra -pedantic $<$<CONFIG:Debug>:-Werror -fsanitize=address> $<$<CONFIG:Release>:-O3>)
target_link_libraries(libbashpp PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
target_include_directories(libbashpp PRIVATE ./bashpp)
target_include_directories(libbashpp PUBLIC .)

add_executable(libbashpp_tests
    tests/command.cpp
    tests/env.cpp
    tests/file_descriptor.cpp
    tests/pipeline.cpp
    tests/visitor.cpp
    tests/wrappers.cpp
)
target_compile_options(libbashpp_tests PRIVATE -Wall -Wextra -pedantic $<$<CONFIG:Debug>:-Werror -fsanitize=address> $<$<CONFIG:Release>:-O3>)
target_link_libraries(libbashpp_tests PRIVATE libbashpp GTest::gtest_main $<$<CONFIG:Debug>:-fsanitize=address>)

gtest_discover_tests(libbashpp_tests)
