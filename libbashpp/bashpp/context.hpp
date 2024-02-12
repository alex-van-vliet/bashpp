#pragma once

#include <bashpp/env.hpp>
#include <bashpp/node.hpp>
#include <ostream>

namespace bashpp {
    class Context {
        Env env_;

    public:
        void print(std::ostream &os, const Node &node) const;

        Env &env() {
            return env_;
        }
        const Env &env() const {
            return env_;
        }
    };
}// namespace bashpp
