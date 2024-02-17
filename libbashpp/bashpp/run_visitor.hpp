#pragma once

#include <bashpp/context.hpp>
#include <bashpp/visitor.hpp>
#include <vector>

namespace bashpp {
    class RunVisitor : public Visitor {
        Context &context_;

    public:
        RunVisitor(Context &context) : context_{context} {}

        static std::vector<const char *> constructArguments(const Command &command);
        static std::vector<const char *> constructEnvironment(const Env &env);

        void visit(Command &command) override;

        void visit(Pipeline &pipeline) override;

    private:
        void start(Command &command, int inPipe = -1, int outPipe = -1);
        void wait(Command &command);
    };
}// namespace bashpp
