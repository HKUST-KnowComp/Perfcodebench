#include "interface.h"
#include "xxhash.h"
#include <cstring>

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* p = input.data();
    const size_t len = input.size();
    const char* const end = p + len;
    constexpr uint64_t mul = 1099511628211ULL;

    while (p < end) {
        const char* line_end = static_cast<const char*>(memchr(p, '\n', end - p));
        if (!line_end) {
            line_end = end;
        }
        const size_t line_len = line_end - p;
        uint64_t h = XXH64(p, line_len, 0);
        acc ^= h;
        acc *= mul;
        p = (line_end < end) ? (line_end + 1) : line_end;
    }
    return acc;
}