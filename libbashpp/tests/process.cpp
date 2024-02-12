#include <bashpp/process.hpp>
#include <gtest/gtest.h>
#include <span>

TEST(LibbashppProcess, ThePidCanBeQueried) {
    bashpp::Process process;
    EXPECT_EQ(process.pid(), 0);
}

TEST(LibbashppProcess, ThePidCanBeUpdated) {
    bashpp::Process process;
    ASSERT_EQ(process.pid(), 0);

    process.pid(10);
    EXPECT_EQ(process.pid(), 10);
}

TEST(LibbashppProcess, TheExitCodeCanBeQueried) {
    bashpp::Process process;
    EXPECT_EQ(process.exit(), 0);
}

TEST(LibbashppProcess, TheExitCodeCanBeUpdated) {
    bashpp::Process process;
    ASSERT_EQ(process.exit(), 0);

    process.exit(10);
    EXPECT_EQ(process.exit(), 10);
}

TEST(LibbashppProcess, ARedirectionCanBeAdded) {
    bashpp::Process process;
    ASSERT_EQ(process.redirections().size(), 0);

    int fd = bashpp::wrappers::dup(1);
    process.addRedirection(1, bashpp::FileDescriptor{fd});

    ASSERT_EQ(process.redirections().size(), 1);

    EXPECT_EQ(process.getRedirectionTempFD(1), fd);
}

TEST(LibbashppProcess, FinishingARedirectionUpdatesTheBuffer) {
    bashpp::Process process;
    ASSERT_EQ(process.redirections().size(), 0);

    int fd = bashpp::wrappers::dup(1);
    process.addRedirection(1, bashpp::FileDescriptor{fd});

    ASSERT_EQ(process.redirections().size(), 1);
    ASSERT_EQ(process.getRedirectionTempFD(1), fd);

    const char *data = "Test";
    std::span<const std::byte> span{reinterpret_cast<const std::byte *>(data), strlen(data)};
    std::vector<std::byte> vector{span.begin(), span.end()};

    process.finishRedirection(1, vector);

    EXPECT_EQ(process.getRedirectionBuffer(1), vector);
}

TEST(LibbashppProcess, FinishingARedirectionClosesTheFile) {
    bashpp::Process process;
    ASSERT_EQ(process.redirections().size(), 0);

    int fd = bashpp::wrappers::dup(1);
    process.addRedirection(1, bashpp::FileDescriptor{fd});

    ASSERT_EQ(process.redirections().size(), 1);
    ASSERT_EQ(process.getRedirectionTempFD(1), fd);

    const char *data = "Test";
    std::span<const std::byte> span{reinterpret_cast<const std::byte *>(data), strlen(data)};
    std::vector<std::byte> vector{span.begin(), span.end()};

    process.finishRedirection(1, vector);

    EXPECT_EQ(process.getRedirectionTempFD(1), -1);
}
