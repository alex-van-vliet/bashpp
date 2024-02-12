#include <bashpp/pipeline.hpp>
#include <gtest/gtest.h>

TEST(LibbashppPipeline, TheCommandsCanBeQueried) {
    bashpp::Pipeline pipeline{{bashpp::Command("echo", {"hello", "world"})}, bashpp::Command("cat", {})};
    ASSERT_EQ(pipeline.commands().size(), 2);
    EXPECT_STREQ(pipeline.commands()[0].command().c_str(), "echo");
    EXPECT_STREQ(pipeline.commands()[1].command().c_str(), "cat");
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
    bashpp::Pipeline pipeline{{bashpp::Command("echo", {"hello", "world"})}, bashpp::Command("cat", {})};
    TestVisitor visitor;
    pipeline.accept(visitor);
    EXPECT_FALSE(visitor.visited_command);
    EXPECT_TRUE(visitor.visited_pipeline);
}

TEST(LibbashppPipeline, ItAcceptsTheConstVisitor) {
    bashpp::Pipeline pipeline{{bashpp::Command("echo", {"hello", "world"})}, bashpp::Command("cat", {})};
    TestConstVisitor visitor;
    pipeline.accept(visitor);
    EXPECT_FALSE(visitor.visited_command);
    EXPECT_TRUE(visitor.visited_pipeline);
}
