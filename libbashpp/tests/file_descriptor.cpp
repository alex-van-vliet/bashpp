#include <bashpp/file_descriptor.hpp>
#include <gtest/gtest.h>

TEST(LibbashppFileDescriptor, TheFdCanBeQueried) {
    EXPECT_EQ(bashpp::FileDescriptor{-1}.fd(), -1);
    int fd = bashpp::wrappers::dup(1);
    EXPECT_EQ(bashpp::FileDescriptor{fd}.fd(), fd);
}

TEST(LibbashppFileDescriptor, TheValidityCanBeChecked) {
    EXPECT_FALSE(bashpp::FileDescriptor{-1}.valid());
    int fd = bashpp::wrappers::dup(1);
    EXPECT_TRUE(bashpp::FileDescriptor{fd}.valid());
}

TEST(LibbashppFileDescriptor, TheFdIsAutomaticallyClosed) {
    int fd = bashpp::wrappers::dup(1);
    {
        bashpp::FileDescriptor file{fd};
        ASSERT_TRUE(file.valid());
        ASSERT_EQ(file.fd(), fd);
    }
    // Checks that the fd has been closed
    EXPECT_EQ(bashpp::wrappers::fcntlGetFD(bashpp::wrappers::Allow{{EBADF}}, fd), -1);
}

TEST(LibbashppFileDescriptor, TheFdCanBeClosed) {
    int fd = bashpp::wrappers::dup(1);
    bashpp::FileDescriptor file{fd};
    ASSERT_TRUE(file.valid());
    ASSERT_EQ(file.fd(), fd);

    file.close();

    ASSERT_FALSE(file.valid());
    ASSERT_EQ(file.fd(), -1);

    // Checks that the fd has been closed
    EXPECT_EQ(bashpp::wrappers::fcntlGetFD(bashpp::wrappers::Allow{{EBADF}}, fd), -1);
}

TEST(LibbashppFileDescriptor, ItCanBeMoveConstructed) {
    int fd = bashpp::wrappers::dup(1);
    bashpp::FileDescriptor source{fd};
    ASSERT_TRUE(source.valid());
    ASSERT_EQ(source.fd(), fd);

    bashpp::FileDescriptor destination{std::move(source)};

    EXPECT_FALSE(source.valid());
    EXPECT_TRUE(destination.valid());
    EXPECT_EQ(source.fd(), -1);
    EXPECT_EQ(destination.fd(), fd);
}

TEST(LibbashppFileDescriptor, ItCanBeMoved) {
    int source_fd = bashpp::wrappers::dup(1);
    int dest_fd = bashpp::wrappers::dup(1);

    bashpp::FileDescriptor source{source_fd};
    ASSERT_TRUE(source.valid());
    ASSERT_EQ(source.fd(), source_fd);

    bashpp::FileDescriptor destination{dest_fd};
    ASSERT_TRUE(destination.valid());
    ASSERT_EQ(destination.fd(), dest_fd);

    destination = std::move(source);

    EXPECT_FALSE(source.valid());
    EXPECT_TRUE(destination.valid());
    EXPECT_EQ(source.fd(), -1);
    EXPECT_EQ(destination.fd(), source_fd);

    // Checks that the dest fd has been closed
    EXPECT_EQ(bashpp::wrappers::fcntlGetFD(bashpp::wrappers::Allow{{EBADF}}, dest_fd), -1);
}
