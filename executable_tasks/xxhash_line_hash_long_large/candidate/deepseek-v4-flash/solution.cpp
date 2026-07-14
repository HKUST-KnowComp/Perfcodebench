#include "interface.h"
#include "xxhash.h"
#include <cstring>

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* p = input.data();
    const char* const end = p + input.size();
    const uint64_t mult = 1099511628211ULL;

    while (p < end) {
        const char* nl = static_cast<const char*>(std::memchr(p, '\n', end - p));
        const char* line_end = nl ? nl : end;
        acc ^= static_cast<uint64_t>(XXH64(p, line_end - p, 0));
        acc *= mult;
        p = nl ? nl + 1 : end;
    }
    return acc;
}
