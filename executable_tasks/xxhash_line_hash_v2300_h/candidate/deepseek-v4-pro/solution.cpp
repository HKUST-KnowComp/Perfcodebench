#include "interface.h"

#include "xxhash.h"

#include <cstring>

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* p = input.data();
    const char* const end = p + input.size();
    while (p < end) {
        const char* line_start = p;
        const char* nl = static_cast<const char*>(std::memchr(p, '\n', end - p));
        if (nl) {
            acc ^= XXH64(line_start, static_cast<size_t>(nl - line_start), 0);
            p = nl + 1;
        } else {
            acc ^= XXH64(line_start, static_cast<size_t>(end - line_start), 0);
            p = end;
        }
        acc *= 1099511628211ULL;
    }
    return acc;
}