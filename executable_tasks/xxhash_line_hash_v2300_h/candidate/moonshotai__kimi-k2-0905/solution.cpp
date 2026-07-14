
#include "interface.h"
#include "xxhash.h"
#include <string>

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 14695981039346656037ULL;
    const char* data = input.data();
    const char* end  = data + input.size();
    const char* line_start = data;

    for (const char* p = data; p < end; ++p) {
        if (*p == '\n') {
            size_t len = p - line_start;
            uint64_t h = XXH64(line_start, len, 0);
            acc ^= h;
            acc *= 1099511628211ULL;
            line_start = p + 1;
        }
    }

    // final line (no trailing newline)
    if (line_start < end) {
        size_t len = end - line_start;
        uint64_t h = XXH64(line_start, len, 0);
        acc ^= h;
        acc *= 1099511628211ULL;
    }

    return acc;
}
