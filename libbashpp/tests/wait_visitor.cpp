#include <bashpp/command.hpp>
#include <bashpp/wait_visitor.hpp>
#include <gtest/gtest.h>

TEST(LibbashppWaitVisitor, ACommandCannotBeWaitedIfNotStarted) {
    bashpp::WaitVisitor visitor;
    bashpp::Command command{"echo", {"hello", "world"}};

    try {
        visitor.visit(command);
        FAIL() << "Expected std::logic_error";
    } catch (const std::logic_error &e) {
        EXPECT_STREQ(e.what(), "Command was not started");
    } catch (...) {
        FAIL() << "Expected std::logic_error";
    }
}

TEST(LibbashppWaitVisitor, ACommandCannotBeWaitedTwice) {
    bashpp::WaitVisitor visitor;
    bashpp::Command command{"echo", {"hello", "world"}};

    // Considering process with pid 0 as finished
    command.process(0);
    try {
        visitor.visit(command);
        FAIL() << "Expected std::logic_error";
    } catch (const std::logic_error &e) {
        EXPECT_STREQ(e.what(), "Command was already waited");
    } catch (...) {
        FAIL() << "Expected std::logic_error";
    }
}
