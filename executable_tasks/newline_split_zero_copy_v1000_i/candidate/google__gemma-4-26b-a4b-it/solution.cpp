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
            // Use find to leverage SIMD-optimized scanning
            size_t end = text.find('\n', start);
            if (end == std::string::npos) {
                end = n;
            }

            size_t len = end - start;
            
            // Update hash with line length
            hash = mix(hash, len);
            
            // Update hash with first character if line is not empty
            if (len > 0) {
                hash = mix(hash, static_cast<unsigned char>(data[start]));
            } else {
                hash = mix(hash, 0u);
            }

            start = end + 1;
        }
        final_hash = hash;
    }

    return final_hash;
}