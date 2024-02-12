#pragma once

#include <bashpp/file_descriptor.hpp>
#include <cstdint>
#include <sys/types.h>
#include <utility>
#include <unordered_map>
#include <vector>

namespace bashpp {
    class Process {
        pid_t pid_;
        std::uint8_t exit_;
        std::unordered_map<int, std::pair<FileDescriptor, std::vector<std::byte>>> capturedRedirections_;

    public:
        Process() : pid_{0}, exit_{0}, capturedRedirections_{} {
        }

        Process(const Process &) = delete;
        Process &operator=(const Process &) = delete;

        Process(Process &&) = default;
        Process &operator=(Process &&) = default;

        pid_t pid() const {
            return pid_;
        }

        void pid(pid_t pid) {
            pid_ = pid;
        }

        std::uint8_t exit() const {
            return exit_;
        }

        void exit(std::uint8_t exit) {
            exit_ = exit;
        }

        const std::unordered_map<int, std::pair<FileDescriptor, std::vector<std::byte>>>& redirections() const {
            return capturedRedirections_;
        }

        void addRedirection(int fd, FileDescriptor tempFd) {
            capturedRedirections_.insert_or_assign(fd,
                                                   std::pair<FileDescriptor, std::vector<std::byte>>{std::move(tempFd), {}});
        }

        void finishRedirection(int fd, std::vector<std::byte> data) {
            auto it = capturedRedirections_.find(fd);
            if (it == capturedRedirections_.end()) {
                return;
            }
            it->second.first.close();
            it->second.second = std::move(data);
        }

        int getRedirectionTempFD(int fd) const {
            auto it = capturedRedirections_.find(fd);
            if (it == capturedRedirections_.end()) {
                return -1;
            }
            return it->second.first.fd();
        }
    };
}// namespace bashpp
