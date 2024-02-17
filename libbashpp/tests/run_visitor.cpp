#include <bashpp/command.hpp>
#include <bashpp/context.hpp>
#include <bashpp/env.hpp>
#include <bashpp/run_visitor.hpp>
#include <gtest/gtest.h>

TEST(LibbashppStartVisitor, TheArgumentsAreConstructedProperly) {
    bashpp::Command command{"echo", {"hello", "world"}};
    auto arguments = bashpp::RunVisitor::constructArguments(command);
    ASSERT_EQ(arguments.size(), 4);
    EXPECT_STREQ(arguments[0], "echo");
    EXPECT_STREQ(arguments[1], "hello");
    EXPECT_STREQ(arguments[2], "world");
    EXPECT_EQ(arguments[3], nullptr);
}

TEST(LibbashppStartVisitor, TheEnvironmentIsConstructedProperly) {
    bashpp::Env env;
    env.clear();
    env.set("HELLO", "WORLD");
    auto environment = bashpp::RunVisitor::constructEnvironment(env);
    ASSERT_EQ(environment.size(), 2);
    EXPECT_STREQ(environment[0], "HELLO=WORLD");
    EXPECT_EQ(environment[1], nullptr);
}

TEST(LibbashppWaitVisitor, ACommandCannotBeStartedTwice) {
    bashpp::Context context;
    bashpp::RunVisitor visitor{context};
    bashpp::Command command{"echo", {"hello", "world"}};

    command.setupProcess();

    try {
        visitor.visit(command);
        FAIL() << "Expected std::logic_error";
    } catch (const std::logic_error &e) {
        EXPECT_STREQ(e.what(), "Command was already started");
    } catch (...) {
        FAIL() << "Expected std::logic_error";
    }
}
