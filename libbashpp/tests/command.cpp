#include <bashpp/command.hpp>
#include <bashpp/visitor.hpp>
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

struct TestVisitor : bashpp::Visitor {
    bool visited_pipeline = false;
    bool visited_command = false;

    void visit(bashpp::Command &) override {
        visited_command = true;
    }

    void visit(bashpp::Pipeline &) override {
        visited_pipeline = true;
    }
};

struct TestConstVisitor : bashpp::ConstVisitor {
    bool visited_pipeline = false;
    bool visited_command = false;

    void visit(const bashpp::Command &) override {
        visited_command = true;
    }

    void visit(const bashpp::Pipeline &) override {
        visited_pipeline = true;
    }
};

TEST(LibbashppCommand, ItAcceptsTheVisitor) {
    bashpp::Command command{"echo", {"hello", "world"}};
    TestVisitor visitor;
    command.accept(visitor);
    EXPECT_TRUE(visitor.visited_command);
    EXPECT_FALSE(visitor.visited_pipeline);
}

TEST(LibbashppCommand, ItAcceptsTheConstVisitor) {
    bashpp::Command command{"echo", {"hello", "world"}};
    TestConstVisitor visitor;
    command.accept(visitor);
    EXPECT_TRUE(visitor.visited_command);
    EXPECT_FALSE(visitor.visited_pipeline);
}
