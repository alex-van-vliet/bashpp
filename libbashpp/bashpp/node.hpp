#pragma once

#include <bashpp/visitor.hpp>

namespace bashpp {
    class Node {
    public:
        virtual ~Node() = default;

        virtual void accept(Visitor &) = 0;
        virtual void accept(ConstVisitor &) const = 0;
    };

    namespace details {
        template<typename T>
        class NodeImpl : public Node {
        public:
            void accept(Visitor &v) override {
                v.visit(static_cast<T &>(*this));
            }
            void accept(ConstVisitor &v) const override {
                v.visit(static_cast<const T &>(*this));
            }
        };
    }// namespace details
}// namespace bashpp