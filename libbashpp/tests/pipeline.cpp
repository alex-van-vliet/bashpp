#include <bashpp/pipeline.hpp>
#include <gtest/gtest.h>

TEST(LibbashppPipeline, TheCommandsCanBeQueried) {
    bashpp::Pipeline pipeline{{bashpp::Command("echo", {"hello", "world"})}, bashpp::Command("cat", {})};
    ASSERT_EQ(pipeline.commands().size(), 2);
    EXPECT_STREQ(pipeline.commands()[0].command().c_str(), "echo");
    EXPECT_STREQ(pipeline.commands()[1].command().c_str(), "cat");
}
