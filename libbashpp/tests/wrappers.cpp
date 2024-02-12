#include <bashpp/wrappers.hpp>
#include <gtest/gtest.h>

int fun(int i, int new_errno) {
    errno = new_errno;
    return i;
}

constexpr bashpp::wrappers::details::Wrapper wrapped{fun};

TEST(LibbashppWrappers, TheWrapperCanBeExecuted) {
    EXPECT_EQ(wrapped(0, EINVAL), 0);
    EXPECT_EQ(wrapped(1, EINVAL), 1);
}

TEST(LibbashppWrappers, TheWrapperCatchesErrors) {
    try {
        wrapped(-1, EINVAL);
        FAIL() << "Expected std::system_error";
    } catch (const std::system_error &e) {
        EXPECT_EQ(e.code().value(), EINVAL);
    } catch (...) {
        FAIL() << "Expected std::system_error";
    }
}

TEST(LibbashppWrappers, TheWrapperCanAcceptSomeErrors) {
    EXPECT_EQ(wrapped(bashpp::wrappers::Allow{{EINVAL}}, -1, EINVAL), -1);
    try {
        wrapped(bashpp::wrappers::Allow{{EAGAIN}}, -1, EINVAL);
        FAIL() << "Expected std::system_error";
    } catch (const std::system_error &e) {
        EXPECT_EQ(e.code().value(), EINVAL);
    } catch (...) {
        FAIL() << "Expected std::system_error";
    }
}

TEST(LibbashppWrappers, TheWrapperCanAcceptAllErrors) {
    EXPECT_EQ(wrapped(bashpp::wrappers::NoThrow{}, -1, EINVAL), -1);
}
