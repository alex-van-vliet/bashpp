#pragma once

namespace bashpp {
    class Command;

    class Pipeline;

    class ConstVisitor {
    public:
        virtual ~ConstVisitor() = default;

        virtual void visit(const Command &) = 0;

        virtual void visit(const Pipeline &) = 0;
    };

    class Visitor {
    public:
        virtual ~Visitor() = default;

        virtual void visit(Command &) = 0;

        virtual void visit(Pipeline &) = 0;
    };
}// namespace bashpp
