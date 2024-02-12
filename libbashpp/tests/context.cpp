#include <bashpp/command.hpp>
#include <bashpp/context.hpp>
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
    std::vector<bashpp::Command> commands;
    commands.emplace_back("echo", std::vector<std::string>{"hello", "world"});
    bashpp::Pipeline pipeline{std::move(commands), bashpp::Command("cat", {})};
    std::stringstream ss;
    context.print(ss, pipeline);

    auto res = ss.str();
    EXPECT_STREQ(res.c_str(), R"("echo" "hello" "world" | "cat")");
}
