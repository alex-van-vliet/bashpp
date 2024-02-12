#include "env.hpp"

extern char **environ;

namespace bashpp {
    Env::Env() {
        for (char **entry = environ; *entry; ++entry) {
            std::string_view entry_sv{*entry};
            auto equals = entry_sv.find('=');
            if (equals == std::string_view::npos) {
                continue;
            }
            env_.emplace(entry_sv.substr(0, equals), entry_sv);
        }
    }
}// namespace bashpp
