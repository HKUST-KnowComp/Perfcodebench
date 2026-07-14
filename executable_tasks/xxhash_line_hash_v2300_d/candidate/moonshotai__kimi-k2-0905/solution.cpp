#include "interface.h"
#include "xxhash.h"
#include <string>
#include <cstdint>

uint64_t hash_lines(const std::string& input) {
    const char* data = input.data();
    const size_t n   = input.size();
    uint64_t acc     = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    size_t pos = 0;
    while (pos < n) {
        const char* line_start = data + pos;
        const char* p = line_start;
        const char* const limit = data + n;

        // Fast unrolled scan for newline
        while (p + 8 <= limit) {
            uint64_t v;
            __builtin_memcpy(&v, p, 8);
            if (has_zero_byte(v)) {
                const char* byte = p;
                while (byte < p + 8) {
                    if (*byte == '\n') goto found;
                    ++byte;
                }
            }
            p += 8;
        }
        while (p < limit && *p != '\n') ++p;
    found:

        size_t len = p - line_start;
        uint64_t h = XXH64(line_start, len, 0);
        acc ^= h;
        acc *= prime;
        pos = (p - data) + 1;  // skip '\n' or set to n+1 if at end
    }
    return acc;
}

static inline int has_zero_byte(uint64_t v) {
    return ((v - 0x0101010101010101ULL) & ~v & 0x8080808080808080ULL) != 0;
}