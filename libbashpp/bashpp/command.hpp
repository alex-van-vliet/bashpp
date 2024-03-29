#pragma once

#include <bashpp/node.hpp>
#include <bashpp/process.hpp>
#include <optional>
#include <span>
#include <string_view>
#include <variant>
#include <vector>

namespace bashpp {
    constexpr int in{0};
    constexpr int out{1};
    constexpr int err{2};

    struct FDRedirection {
        int fd;
    };
    struct CloseFDRedirection {};
    struct InputPathRedirection {
        std::string path;
    };
    struct OutputPathRedirection {
        std::string path;
    };
    struct OutputPathAppendRedirection {
        std::string path;
    };
    struct InputOutputPathRedirection {
        std::string path;
    };
    struct InputVariableRedirection {
        std::vector<std::byte> bytes;
    };
    struct OutputVariableRedirection {};

    struct Redirection {
        int fd;
        std::variant<FDRedirection, CloseFDRedirection,
                     InputPathRedirection, OutputPathRedirection, OutputPathAppendRedirection,
                     InputOutputPathRedirection,
                     InputVariableRedirection, OutputVariableRedirection>
                redirection;
    };

    class Command : public details::NodeImpl<Command> {
        std::string program_;
        std::vector<std::string> arguments_;
        std::vector<Redirection> redirections_;
        std::optional<Process> process_;

    public:
        explicit Command(std::string program,
                         std::vector<std::string> arguments = {},
                         std::vector<Redirection> redirections = {})
            : program_{std::move(program)}, arguments_{std::move(arguments)}, redirections_{std::move(redirections)},
              process_{std::nullopt} {}

        Command(const Command &) = delete;
        Command &operator=(const Command &) = delete;

        Command(Command &&) = default;
        Command &operator=(Command &&) = default;

        const std::string &program() const {
            return program_;
        }

        const std::vector<std::string> &arguments() const {
            return arguments_;
        }

        std::vector<Redirection> &redirections() {
            return redirections_;
        }

        const std::vector<Redirection> &redirections() const {
            return redirections_;
        }

        std::optional<Process> &process() {
            return process_;
        }

        const std::optional<Process> &process() const {
            return process_;
        }

        void setupProcess() {
            process_.emplace();
        }

        pid_t exit() const override {
            if (!process_) {
                throw std::logic_error{"Command was not started"};
            }
            if (process_->pid() != 0) {
                throw std::logic_error{"Command was not waited"};
            }
            return process_->exit();
        }

        const std::vector<std::byte> &redirection(int fd) const override {
            if (!process_) {
                throw std::logic_error{"Command was not started"};
            }
            if (process_->pid() != 0) {
                throw std::logic_error{"Command was not waited"};
            }
            return process_->getRedirectionBuffer(fd);
        }
    };
}// namespace bashpp
