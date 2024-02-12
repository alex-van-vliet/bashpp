#include "node.hpp"

#include <bashpp/start_visitor.hpp>
#include <bashpp/wait_visitor.hpp>

namespace bashpp {
    void Node::start(Context &context) {
        StartVisitor visitor{context};
        accept(visitor);
    }

    void Node::wait() {
        WaitVisitor visitor;
        accept(visitor);
    }
}// namespace bashpp
