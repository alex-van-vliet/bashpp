#include <bashpp/command.hpp>
#include <gtest/gtest.h>

TEST(LibbashppCommand, TheCommandCanBeQueried) {
    bashpp::Command command{"echo", {"hello", "world"}};
    EXPECT_STREQ(command.command().c_str(), "echo");
}

TEST(LibbashppCommand, TheArgumentsCanBeQueried) {
    bashpp::Command command{"echo", {"hello", "world"}};
    ASSERT_EQ(command.arguments().size(), 2);
    EXPECT_STREQ(command.arguments()[0].c_str(), "hello");
    EXPECT_STREQ(command.arguments()[1].c_str(), "world");
}
