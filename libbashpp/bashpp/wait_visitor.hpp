#pragma once

#include <bashpp/visitor.hpp>
#include <vector>

namespace bashpp {
    class WaitVisitor : public Visitor {
    public:
        void visit(Command &command) override;

        void visit(Pipeline &pipeline) override;
    };
}// namespace bashpp
