#pragma once

#include <bashpp/node.hpp>
#include <bashpp/process.hpp>
#include <optional>
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

    struct Redirection {
        int fd;
        std::variant<FDRedirection, CloseFDRedirection> redirection;
    };

    class Command : public details::NodeImpl<Command> {
        std::string program_;
        std::vector<std::string> arguments_;
        std::vector<Redirection> redirections_;
        std::optional<Process> process_;

    public:
        Command(std::string program, std::vector<std::string> arguments, std::vector<Redirection> redirections = {})
            : program_{std::move(program)}, arguments_{std::move(arguments)}, redirections_{std::move(redirections)},
              process_{std::nullopt} {}

        const std::string &program() const {
            return program_;
        }

        const std::vector<std::string> &arguments() const {
            return arguments_;
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

        void process(pid_t pid) {
            process_ = Process{pid};
        }
    };
}// namespace bashpp
