#include "interface.h"
#include <cstring>
#include <cstdint>

uint64_t newline_split_checksum(const std::string& text, int iters) {
    const char* data = text.data();
    const size_t len = text.size();
    uint64_t hash = 0;
    constexpr uint64_t FNV_PRIME = 1099511628211ULL;
    constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        size_t start = 0;
        while (start < len) {
            const char* nl = static_cast<const char*>(std::memchr(data + start, '\n', len - start));
            size_t end = nl ? (nl - data) : len;
            size_t line_len = end - start;
            
            hash ^= line_len;
            hash *= FNV_PRIME;
            hash ^= (line_len > 0) ? static_cast<unsigned char>(data[start]) : 0u;
            hash *= FNV_PRIME;
            
            start = end + 1;
        }
    }
    return hash;
}
