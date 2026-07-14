#include "interface.h"

#include <string>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
    const char* data = text.data();
    const size_t n = text.size();
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = 1469598103934665603ULL;
        size_t start = 0;
        
        while (start < n) {
            size_t end = start;
            // Manual scan for newline to avoid std::string::find overhead
            // and allow for better compiler vectorization/unrolling.
            while (end < n && data[end] != '\n') {
                end++;
            }

            size_t len = end - start;
            // Update hash with line length
            hash = mix(hash, len);
            // Update hash with first character (if line not empty)
            hash = mix(hash, len == 0 ? 0u : static_cast<unsigned char>(data[start]));

            start = end + 1;
        }
        final_hash = hash;
    }

    return final_hash;
}