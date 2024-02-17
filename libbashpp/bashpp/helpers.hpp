#pragma once

#include <span>
#include <string_view>

namespace bashpp {
    std::span<const std::byte> svToByteSpan(std::string_view sv) {
        return {reinterpret_cast<const std::byte *>(sv.data()), sv.size()};
    }
    std::string_view byteSpanToSv(std::span<const std::byte> bytes) {
        return {reinterpret_cast<const char *>(bytes.data()), bytes.size()};
    }
}// namespace bashpp
