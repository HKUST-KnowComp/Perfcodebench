#include "interface.h"
#include <string>
#include <cstdint>

uint64_t newline_split_checksum(const std::string& text, int iters) {
    const char* data = text.data();
    const char* end_ptr = data + text.size();
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        const char* line_start = data;
        const char* ptr = data;

        while (ptr < end_ptr) {
            if (*ptr == '\n') {
                uint64_t line_len = static_cast<uint64_t>(ptr - line_start);
                hash ^= line_len;
                hash *= 1099511628211ULL;
                uint64_t first = (line_len == 0) ? 0 : static_cast<unsigned char>(*line_start);
                hash ^= first;
                hash *= 1099511628211ULL;
                line_start = ptr + 1;
            }
            ++ptr;
        }
        if (line_start < end_ptr) {
            uint64_t line_len = static_cast<uint64_t>(end_ptr - line_start);
            hash ^= line_len;
            hash *= 1099511628211ULL;
            uint64_t first = (line_len == 0) ? 0 : static_cast<unsigned char>(*line_start);
            hash ^= first;
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}