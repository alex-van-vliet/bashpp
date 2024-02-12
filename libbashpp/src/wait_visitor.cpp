#include "wait_visitor.hpp"

#include <bashpp/command.hpp>
#include <bashpp/pipeline.hpp>
#include <bashpp/wrappers.hpp>
#include <iostream>

namespace {
    using namespace bashpp;

    class RedirectionTeardownVisitor {
        int fd_;
        Command &command_;

    public:
        RedirectionTeardownVisitor(int fd, Command &command) : fd_{fd}, command_{command} {}

        void operator()(const OutputVariableRedirection &) {
            int fd = command_.process()->getRedirectionTempFD(fd_);

            wrappers::lseek(fd, SEEK_SET, 0);

            constexpr std::size_t BUFFER_SIZE = 2048;

            std::vector<std::byte> buffer;

            buffer.resize(BUFFER_SIZE);
            std::size_t read = wrappers::read(fd, buffer.data(), BUFFER_SIZE);
            while (read == BUFFER_SIZE) {
                std::size_t current_size = buffer.size();
                buffer.resize(current_size + BUFFER_SIZE);
                read = wrappers::read(fd, buffer.data() + current_size, BUFFER_SIZE);
            }
            buffer.resize(buffer.size() + read - BUFFER_SIZE);

            command_.process()->finishRedirection(fd_, std::move(buffer));
        }
        void operator()(const auto &) {
        }
    };
}

namespace bashpp {
    void WaitVisitor::visit(Command &command) {
        if (!command.process()) {
            throw std::logic_error{"Command was not started"};
        }
        if (command.process()->pid() == 0) {
            throw std::logic_error{"Command was already waited"};
        }

        int status;
        wrappers::waitpid(command.process()->pid(), &status, 0);
        command.process()->pid(0);

        if (WIFEXITED(status)) {
            command.process()->exit(WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            command.process()->exit(128 + WTERMSIG(status));
        } else {
            // Should not happen, leave 0
        }
        for (const auto &redirection: command.redirections()) {
            std::visit(RedirectionTeardownVisitor{redirection.fd, command}, redirection.redirection);
        }
    }

    void WaitVisitor::visit(Pipeline &pipeline) {
        (void)pipeline;
    }
}// namespace bashpp
