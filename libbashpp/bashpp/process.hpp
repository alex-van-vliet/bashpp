#pragma once

#include <cstdint>
#include <sys/types.h>

namespace bashpp {
    class Process {
        pid_t pid_;
        std::uint8_t exit_;

        friend class Command;

    public:
        Process(pid_t pid) : pid_{pid}, exit_{0} {
        }

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
    };
}// namespace bashpp
