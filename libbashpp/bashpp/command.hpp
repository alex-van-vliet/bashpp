#pragma once

#include <bashpp/node.hpp>
#include <vector>

namespace bashpp {
    class Command : public details::NodeImpl<Command> {
        std::string program_;
        std::vector<std::string> arguments_;

    public:
        Command(std::string program, std::vector<std::string> arguments)
            : program_{std::move(program)}, arguments_{std::move(arguments)} {}

        const std::string &program() const {
            return program_;
        }

        const std::vector<std::string> &arguments() const {
            return arguments_;
        }
    };
}// namespace bashpp
