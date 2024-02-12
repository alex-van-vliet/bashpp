add_library(libbashpp
    bashpp/command.hpp src/command.cpp
    bashpp/context.hpp src/context.cpp
    bashpp/env.hpp src/env.cpp
    bashpp/file_descriptor.hpp src/file_descriptor.cpp
    bashpp/node.hpp src/node.cpp
    bashpp/pipeline.hpp src/pipeline.cpp
    bashpp/process.hpp src/process.cpp
    bashpp/start_visitor.hpp src/start_visitor.cpp
    bashpp/visitor.hpp src/visitor.cpp
    bashpp/wait_visitor.hpp src/wait_visitor.cpp
    bashpp/wrappers.hpp src/wrappers.cpp
)
target_compile_options(libbashpp PRIVATE -Wall -Wextra -pedantic $<$<CONFIG:Debug>:-Werror -fsanitize=address> $<$<CONFIG:Release>:-O3>)
target_link_libraries(libbashpp PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
target_include_directories(libbashpp PRIVATE ./bashpp)
target_include_directories(libbashpp PUBLIC .)

add_executable(libbashpp_tests
    tests/command.cpp
    tests/context.cpp
    tests/env.cpp
    tests/file_descriptor.cpp
    tests/pipeline.cpp
    tests/start_visitor.cpp
    tests/visitor.cpp
    tests/wait_visitor.cpp
    tests/wrappers.cpp
)
target_compile_options(libbashpp_tests PRIVATE -Wall -Wextra -pedantic $<$<CONFIG:Debug>:-Werror -fsanitize=address> $<$<CONFIG:Release>:-O3>)
target_link_libraries(libbashpp_tests PRIVATE libbashpp GTest::gtest_main $<$<CONFIG:Debug>:-fsanitize=address>)

gtest_discover_tests(libbashpp_tests)

add_executable(libbashpp_integration integration/main.cpp)
target_compile_options(libbashpp_integration PRIVATE -Wall -Wextra -pedantic $<$<CONFIG:Debug>:-Werror -fsanitize=address> $<$<CONFIG:Release>:-O3>)
target_link_libraries(libbashpp_integration PRIVATE libbashpp $<$<CONFIG:Debug>:-fsanitize=address>)

pytest_discover_tests(libbashpp_integration_tests
    TRIM_FROM_NAME "^test_"
    DEPENDS libbashpp_integration
    ENVIRONMENT
        BASHPP_INTEGRATION_EXE=$<TARGET_FILE:libbashpp_integration>
)
