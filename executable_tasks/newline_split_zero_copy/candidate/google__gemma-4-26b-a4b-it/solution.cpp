#include "interface.h"
#include <string>
#include <cstring>

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
            // Use memchr for highly optimized newline scanning
            const char* next_nl = static_cast<const char*>(std::memchr(data + start, '\n', n - start));
            size_t end = next_nl ? (next_nl - data) : n;
            size_t len = end - start;

            // Update hash using the logic from the baseline
            hash = mix(hash, len);
            hash = mix(hash, len == 0 ? 0u : static_cast<unsigned char>(data[start]));

            if (next_nl) {
                start = (next_nl - data) + 1;
            } else {
                break;
            }
        }
        final_hash = hash;
    }

    return final_hash;
}
