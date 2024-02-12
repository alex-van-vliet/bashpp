#pragma once

#include <bashpp/env.hpp>
#include <bashpp/node.hpp>
#include <ostream>

namespace bashpp {
    class Context {
    public:
        void print(std::ostream &os, const Node &node) const;
    };
}// namespace bashpp
