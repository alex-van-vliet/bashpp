#include <bashpp/command.hpp>
#include <bashpp/pipeline.hpp>
#include <bashpp/visitor.hpp>
#include <gtest/gtest.h>

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

TEST(LibbashppVisitor, TheVisitorVisitsTheCommand) {
    TestVisitor visitor;
    bashpp::Command command{"echo", {"hello", "world"}};
    visitor.visit(command);
    EXPECT_TRUE(visitor.visited_command);
    EXPECT_FALSE(visitor.visited_pipeline);
}

TEST(LibbashppVisitor, TheVisitorVisitsThePipeline) {
    TestVisitor visitor;
    std::vector<bashpp::Command> commands;
    commands.emplace_back("echo", std::vector<std::string>{"hello", "world"});
    bashpp::Pipeline pipeline{std::move(commands), bashpp::Command("cat", {})};
    visitor.visit(pipeline);
    EXPECT_FALSE(visitor.visited_command);
    EXPECT_TRUE(visitor.visited_pipeline);
}

TEST(LibbashppVisitor, TheConstVisitorVisitsTheCommand) {
    TestConstVisitor visitor;
    bashpp::Command command{"echo", {"hello", "world"}};
    visitor.visit(command);
    EXPECT_TRUE(visitor.visited_command);
    EXPECT_FALSE(visitor.visited_pipeline);
}

TEST(LibbashppVisitor, TheConstVisitorVisitsThePipeline) {
    TestConstVisitor visitor;
    std::vector<bashpp::Command> commands;
    commands.emplace_back("echo", std::vector<std::string>{"hello", "world"});
    bashpp::Pipeline pipeline{std::move(commands), bashpp::Command("cat", {})};
    visitor.visit(pipeline);
    EXPECT_FALSE(visitor.visited_command);
    EXPECT_TRUE(visitor.visited_pipeline);
}
