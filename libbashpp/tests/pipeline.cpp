#include <bashpp/pipeline.hpp>
#include <gtest/gtest.h>

TEST(LibbashppPipeline, TheCommandsCanBeQueried) {
    std::vector<bashpp::Command> commands;
    commands.emplace_back("echo", std::vector<std::string>{"hello", "world"});
    bashpp::Pipeline pipeline{std::move(commands), bashpp::Command("cat", {})};
    ASSERT_EQ(pipeline.commands().size(), 2);
    EXPECT_STREQ(pipeline.commands()[0].program().c_str(), "echo");
    EXPECT_STREQ(pipeline.commands()[1].program().c_str(), "cat");
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

TEST(LibbashppPipeline, ItAcceptsTheVisitor) {
    std::vector<bashpp::Command> commands;
    commands.emplace_back("echo", std::vector<std::string>{"hello", "world"});
    bashpp::Pipeline pipeline{std::move(commands), bashpp::Command("cat", {})};
    TestVisitor visitor;
    pipeline.accept(visitor);
    EXPECT_FALSE(visitor.visited_command);
    EXPECT_TRUE(visitor.visited_pipeline);
}

TEST(LibbashppPipeline, ItAcceptsTheConstVisitor) {
    std::vector<bashpp::Command> commands;
    commands.emplace_back("echo", std::vector<std::string>{"hello", "world"});
    bashpp::Pipeline pipeline{std::move(commands), bashpp::Command("cat", {})};
    TestConstVisitor visitor;
    pipeline.accept(visitor);
    EXPECT_FALSE(visitor.visited_command);
    EXPECT_TRUE(visitor.visited_pipeline);
}
