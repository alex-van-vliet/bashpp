#include <bashpp/command.hpp>
#include <bashpp/visitor.hpp>
#include <gtest/gtest.h>
#include <span>

TEST(LibbashppCommand, TheProgramCanBeQueried) {
    bashpp::Command command{"echo", {"hello", "world"}};
    EXPECT_STREQ(command.program().c_str(), "echo");
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

TEST(LibbashppCommand, TheExitCodeCanBeQueried) {
    bashpp::Command command{"echo", {"hello", "world"}};
    command.setupProcess();
    // Considering process with pid 0 as finished
    command.process()->exit(10);

    EXPECT_EQ(command.exit(), 10);
}

TEST(LibbashppCommand, GettingTheExitCodeThrowsBeforeStart) {
    bashpp::Command command{"echo", {"hello", "world"}};
    try {
        command.exit();
        FAIL() << "Expected std::logic_error";
    } catch (const std::logic_error &e) {
        EXPECT_STREQ(e.what(), "Command was not started");
    } catch (...) {
        FAIL() << "Expected std::logic_error";
    }
}

TEST(LibbashppCommand, GettingTheExitCodeThrowsBeforeWait) {
    bashpp::Command command{"echo", {"hello", "world"}};
    command.setupProcess();
    command.process()->pid(1);
    try {
        command.exit();
        FAIL() << "Expected std::logic_error";
    } catch (const std::logic_error &e) {
        EXPECT_STREQ(e.what(), "Command was not waited");
    } catch (...) {
        FAIL() << "Expected std::logic_error";
    }
}

TEST(LibbashppCommand, ARedirectionBufferCanBeQueried) {
    bashpp::Command command{"echo", {"hello", "world"}};
    command.setupProcess();
    // Considering process with pid 0 as finished
    command.process()->exit(10);
    command.process()->addRedirection(1, bashpp::FileDescriptor{-1});
    const char *data = "Test";
    std::span<const std::byte> span{reinterpret_cast<const std::byte *>(data), strlen(data)};
    std::vector<std::byte> vector{span.begin(), span.end()};
    command.process()->finishRedirection(1, vector);

    EXPECT_EQ(command.redirection(1), vector);
}

TEST(LibbashppCommand, GettingARedirectionBufferThrowsBeforeStart) {
    bashpp::Command command{"echo", {"hello", "world"}};
    try {
        command.redirection(1);
        FAIL() << "Expected std::logic_error";
    } catch (const std::logic_error &e) {
        EXPECT_STREQ(e.what(), "Command was not started");
    } catch (...) {
        FAIL() << "Expected std::logic_error";
    }
}

TEST(LibbashppCommand, GettingARedirectionBufferThrowsBeforeWait) {
    bashpp::Command command{"echo", {"hello", "world"}};
    command.setupProcess();
    command.process()->pid(1);
    try {
        command.redirection(1);
        FAIL() << "Expected std::logic_error";
    } catch (const std::logic_error &e) {
        EXPECT_STREQ(e.what(), "Command was not waited");
    } catch (...) {
        FAIL() << "Expected std::logic_error";
    }
}
