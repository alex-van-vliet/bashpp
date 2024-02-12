#pragma once

#include <bashpp/visitor.hpp>
#include <cstddef>
#include <vector>

namespace bashpp {
    class Context;

    class Node {
    public:
        virtual ~Node() = default;

        virtual void accept(Visitor &) = 0;
        virtual void accept(ConstVisitor &) const = 0;

        void start(Context &context);
        void wait();

        virtual int exit() const = 0;
        virtual const std::vector<std::byte>& redirection(int fd) const = 0;
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
