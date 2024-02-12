#pragma once

#include <bashpp/command.hpp>
#include <bashpp/file_descriptor.hpp>
#include <bashpp/node.hpp>
#include <vector>

namespace bashpp {
    class Pipeline : public details::NodeImpl<Pipeline> {
        std::vector<Command> commands_;

    public:
        Pipeline(std::vector<Command> commands, Command last) : commands_{std::move(commands)} {
            commands_.emplace_back(std::move(last));
        }

        Pipeline(const Pipeline &) = delete;
        Pipeline &operator=(const Pipeline &) = delete;

        Pipeline(Pipeline &&) = default;
        Pipeline &operator=(Pipeline &&) = default;

        std::vector<Command> &commands() {
            return commands_;
        }
        const std::vector<Command> &commands() const {
            return commands_;
        }

        void pushCommand(Command command) {
            commands_.emplace_back(std::move(command));
        }

        pid_t exit() const override {
            return commands_.back().exit();
        }

        const std::vector<std::byte> &redirection(int fd) const override {
            return commands_.back().redirection(fd);
        }
    };

    inline Pipeline operator|(Command first, Command second) {
        std::vector<Command> commands;
        commands.emplace_back(std::move(first));
        return {std::move(commands), std::move(second)};
    }

    inline Pipeline operator|(Pipeline first, Command second) {
        first.pushCommand(std::move(second));
        return first;
    }
}// namespace bashpp
