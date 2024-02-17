#include "node.hpp"

#include <bashpp/run_visitor.hpp>

namespace bashpp {
    void Node::run(Context &context) {
        RunVisitor visitor{context};
        accept(visitor);
    }
}// namespace bashpp
