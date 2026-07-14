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
            const char* next_ptr = static_cast<const char*>(std::memchr(data + start, '\n', n - start));
            size_t end = next_ptr ? (static_cast<size_t>(next_ptr - data)) : n;
            
            size_t line_len = end - start;
            
            // Mix line length
            hash = mix(hash, line_len);
            
            // Mix first character
            if (line_len > 0) {
                hash = mix(hash, static_cast<unsigned char>(data[start]));
            } else {
                hash = mix(hash, 0u);
            }

            if (next_ptr) {
                start = (next_ptr - data) + 1;
            } else {
                break;
            }
        }
        final_hash = hash;
    }

    return final_hash;
}
