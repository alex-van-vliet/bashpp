add_library(libbashpp
    bashpp/process.hpp src/process.cpp
)
target_compile_options(libbashpp PRIVATE -Wall -Wextra -pedantic $<$<CONFIG:Debug>:-Werror -fsanitize=address> $<$<CONFIG:Release>:-O3>)
target_link_libraries(libbashpp PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
target_include_directories(libbashpp PRIVATE ./bashpp)
target_include_directories(libbashpp PUBLIC .)
