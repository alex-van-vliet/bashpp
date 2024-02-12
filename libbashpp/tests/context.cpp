#include <bashpp/context.hpp>
#include <bashpp/command.hpp>
#include <bashpp/pipeline.hpp>
#include <gtest/gtest.h>
#include <sstream>

TEST(LibbashppContext, TheCommandCanBePrinted) {
    bashpp::Context context;
    bashpp::Command command{"echo", {"hello", "world"}};
    std::stringstream ss;
    context.print(ss, command);

    auto res = ss.str();
    EXPECT_STREQ(res.c_str(), R"("echo" "hello" "world")");
}

TEST(LibbashppContext, ThePipelineCanBePrinted) {
    bashpp::Context context;
    bashpp::Pipeline pipeline{{bashpp::Command("echo", {"hello", "world"})}, bashpp::Command("cat", {})};
    std::stringstream ss;
    context.print(ss, pipeline);

    auto res = ss.str();
    EXPECT_STREQ(res.c_str(), R"("echo" "hello" "world" | "cat")");
}
