#pragma once

#include <bashpp/context.hpp>
#include <bashpp/visitor.hpp>
#include <vector>

namespace bashpp {
    class StartVisitor : public Visitor {
        Context &context_;

    public:
        StartVisitor(Context &context) : context_{context} {}

        static std::vector<const char *> constructArguments(const Command &command);
        static std::vector<const char *> constructEnvironment(const Env &env);

        void visit(Command &command) override;

        void visit(Pipeline &pipeline) override;
    };
}// namespace bashpp
