#pragma once

#include <bashpp/command.hpp>
#include <bashpp/node.hpp>
#include <vector>

namespace bashpp {
    class Pipeline : public details::NodeImpl<Pipeline> {
        std::vector<Command> commands_;

    public:
        Pipeline(std::vector<Command> commands, Command last) : commands_{std::move(commands)} {
            commands_.emplace_back(std::move(last));
        }

        const std::vector<Command> &commands() const {
            return commands_;
        }
    };
}// namespace bashpp
