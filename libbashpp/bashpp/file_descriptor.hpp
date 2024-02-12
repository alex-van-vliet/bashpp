#pragma once

#include <bashpp/wrappers.hpp>

namespace bashpp {
    class FileDescriptor {
        int fd_;

    public:
        FileDescriptor() : fd_{-1} {}
        explicit FileDescriptor(int fd) : fd_{fd} {}

        ~FileDescriptor() {
            close();
        }

        FileDescriptor(const FileDescriptor &) = delete;
        FileDescriptor &operator=(const FileDescriptor &) = delete;

        FileDescriptor(FileDescriptor &&other) noexcept : fd_{std::exchange(other.fd_, -1)} {}
        FileDescriptor &operator=(FileDescriptor &&other) noexcept {
            if (&other != this) {
                close();
                fd_ = std::exchange(other.fd_, -1);
            }
            return *this;
        }

        void close() {
            if (valid()) {
                wrappers::close(wrappers::NoThrow{}, fd_);
                fd_ = -1;
            }
        }

        int fd() const {
            return fd_;
        }

        bool valid() const {
            return fd_ > 0;
        }
    };
}// namespace bashpp
