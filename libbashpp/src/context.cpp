#include "context.hpp"

#include <bashpp/command.hpp>
#include <bashpp/pipeline.hpp>
#include <bashpp/visitor.hpp>
#include <iomanip>

namespace bashpp {
    class PrintVisitor : public ConstVisitor {
        std::ostream &os_;

    public:
        explicit PrintVisitor(std::ostream &os) : os_{os} {}

        void operator()(const Node &n) {
            n.accept(*this);
        }

        void visit(const Command &command) override {
            os_ << std::quoted(command.program());
            for (const auto &argument: command.arguments()) {
                os_ << ' ' << std::quoted(argument);
            }
        }

        void visit(const Pipeline &op) override {
            visit(op.commands()[0]);
            for (std::size_t i = 1; i < op.commands().size(); ++i) {
                os_ << " | ";
                visit(op.commands()[i]);
            }
        }
    };

    void Context::print(std::ostream &os, const bashpp::Node &node) const {
        PrintVisitor visitor{os};
        visitor(node);
    }
}// namespace bashpp
