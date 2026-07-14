#include "interface.h"
#include "xxhash.h"
#include <cstring>

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* p   = input.data();
    const char* end = p + input.size();

    while (p < end) {
        const void* nl = ::memchr(p, '\n', static_cast<size_t>(end - p));
        const char* line_end = (nl) ? static_cast<const char*>(nl) : end;
        size_t len = static_cast<size_t>(line_end - p);

        uint64_t h = XXH64(p, len, 0);
        acc ^= h;
        acc *= 1099511628211ULL;

        p = (line_end < end) ? line_end + 1 : end;
    }
    return acc;
}