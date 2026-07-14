#include "interface.h"

#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
    if (iters <= 0) {
        return 0;
    }

    uint64_t hash = 1469598104665603ULL;
    const char* data = text.data();
    std::size_t size = text.size();
    std::size_t line_start = 0;

    for (std::size_t i = 0; i < size; ++i) {
        if (data[i] == '\n') {
            std::size_t len = i - line_start;
            hash = mix(hash, len);
            hash = mix(hash, (len == 0) ? 0u : static_cast<unsigned char>(data[line_start]));
            line_start = i + 1;
        }
    }

    // Handle final line when input does not end with newline
    if (line_start < size) {
        std::size_t len = size - line_start;
        hash = mix(hash, len);
        hash = mix(hash, (len == 0) ? 0u : static_cast<unsigned char>(data[line_start]));
    }

    return hash;
}