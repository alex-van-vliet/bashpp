#pragma once

#include <bashpp/node.hpp>
#include <bashpp/process.hpp>
#include <optional>
#include <vector>

namespace bashpp {
    class Command : public details::NodeImpl<Command> {
        std::string program_;
        std::vector<std::string> arguments_;
        std::optional<Process> process_;

    public:
        Command(std::string program, std::vector<std::string> arguments)
            : program_{std::move(program)}, arguments_{std::move(arguments)}, process_{std::nullopt} {}

        const std::string &program() const {
            return program_;
        }

        const std::vector<std::string> &arguments() const {
            return arguments_;
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
