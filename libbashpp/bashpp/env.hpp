#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

namespace bashpp {
    namespace details {
        struct string_hash {
            using is_transparent = void;

            std::size_t operator()(std::string_view key) const {
                return std::hash<std::string_view>{}(key);
            }
            std::size_t operator()(const std::string &key) const {
                return std::hash<std::string_view>{}(key);
            }
        };
    }// namespace details

    class Env {
        // Key => Entry (KEY=VALUE)
        std::unordered_map<std::string, std::string, details::string_hash, std::equal_to<>> env_;

    public:
        Env();

        std::string_view get(std::string_view key) const {
            auto it = env_.find(key);
            if (it == env_.end()) {
                return {""};
            }
            return std::string_view{it->second}.substr(it->first.size() + 1);
        }

        bool contains(std::string_view key) const {
            return env_.contains(key);
        }

        void set(std::string key, std::string_view value) {
            std::string valueToInsert{key};
            valueToInsert += "=";
            valueToInsert += value;
            env_.insert_or_assign(std::move(key), std::move(valueToInsert));
        }
    };
}// namespace bashpp
