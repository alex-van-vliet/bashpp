#include <bashpp/pipeline.hpp>
#include <gtest/gtest.h>
#include <span>

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

TEST(LibbashppPipeline, ACommandCanBeAdded) {
    std::vector<bashpp::Command> commands;
    commands.emplace_back("echo", std::vector<std::string>{"hello", "world"});
    bashpp::Pipeline pipeline{std::move(commands), bashpp::Command("cat", {})};
    pipeline.pushCommand(bashpp::Command("grep", {"world"}));

    ASSERT_EQ(pipeline.commands().size(), 3);
    EXPECT_STREQ(pipeline.commands()[0].program().c_str(), "echo");
    EXPECT_STREQ(pipeline.commands()[1].program().c_str(), "cat");
    EXPECT_STREQ(pipeline.commands()[2].program().c_str(), "grep");
}

TEST(LibbashppPipeline, APipelineCanBeCreatedWithTwoCommandsAndAPipe) {
    auto pipeline = bashpp::Command{"echo", {"hello", "world"}} | bashpp::Command{"cat", {}};

    ASSERT_EQ(pipeline.commands().size(), 2);
    EXPECT_STREQ(pipeline.commands()[0].program().c_str(), "echo");
    EXPECT_STREQ(pipeline.commands()[1].program().c_str(), "cat");
}

TEST(LibbashppPipeline, ACommandCanBeAddedWithAPipe) {
    auto first_two = bashpp::Command{"echo", {"hello", "world"}} | bashpp::Command{"cat", {}};
    auto pipeline = std::move(first_two) | bashpp::Command{"grep", {"world"}};

    ASSERT_EQ(pipeline.commands().size(), 3);
    EXPECT_STREQ(pipeline.commands()[0].program().c_str(), "echo");
    EXPECT_STREQ(pipeline.commands()[1].program().c_str(), "cat");
    EXPECT_STREQ(pipeline.commands()[2].program().c_str(), "grep");
}

TEST(LibbashppPipeline, TheExitCodeIsTheExitCodeOfTheLastCommand) {
    auto pipeline = bashpp::Command{"echo", {"hello", "world"}} | bashpp::Command{"cat", {}};
    pipeline.commands().back().setupProcess();
    // Considering process with pid 0 as finished
    pipeline.commands().back().process()->exit(10);

    EXPECT_EQ(pipeline.exit(), 10);
}

TEST(LibbashppPipeline, TheRedirectionsAreTheRedirectionsOfTheLastCommand) {
    auto pipeline = bashpp::Command{"echo", {"hello", "world"}} | bashpp::Command{"cat", {}};
    pipeline.commands().back().setupProcess();
    // Considering process with pid 0 as finished
    pipeline.commands().back().process()->addRedirection(1, bashpp::FileDescriptor{});
    const char *data = "Test";
    std::span<const std::byte> span{reinterpret_cast<const std::byte *>(data), strlen(data)};
    std::vector<std::byte> vector{span.begin(), span.end()};
    pipeline.commands().back().process()->finishRedirection(1, vector);

    EXPECT_EQ(pipeline.redirection(1), vector);
}
