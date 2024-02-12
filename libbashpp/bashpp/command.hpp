#pragma once

#include <bashpp/node.hpp>
#include <vector>

namespace bashpp {
    class Command : public details::NodeImpl<Command> {
        std::string command_;
        std::vector<std::string> arguments_;

    public:
        Command(std::string command, std::vector<std::string> arguments)
            : command_{std::move(command)}, arguments_{std::move(arguments)} {}

        const std::string &command() const {
            return command_;
        }

        const std::vector<std::string> &arguments() const {
            return arguments_;
        }
    };
}// namespace bashpp
