#include "wait_visitor.hpp"

#include <bashpp/command.hpp>
#include <bashpp/pipeline.hpp>
#include <bashpp/wrappers.hpp>
#include <iostream>

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
    }

    void WaitVisitor::visit(Pipeline &pipeline) {
        (void)pipeline;
    }
}// namespace bashpp
