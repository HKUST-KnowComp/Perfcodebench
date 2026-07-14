#include "interface.h"
#include "xxhash.h"
#include <cstring>   // for memchr

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* pos = input.data();
    const char* const end = pos + input.size();

    while (pos < end) {
        // Find the next newline using memchr (fast SIMD-enabled search)
        const char* nl = static_cast<const char*>(
            memchr(pos, '\n', static_cast<size_t>(end - pos))
        );

        size_t line_len;
        if (nl) {
            line_len = static_cast<size_t>(nl - pos);
            pos = nl + 1;   // skip the newline
        } else {
            // Last line (no trailing newline)
            line_len = static_cast<size_t>(end - pos);
            pos = end;
        }

        acc ^= static_cast<uint64_t>(XXH64(pos - (nl ? line_len + 1 : line_len), line_len, 0));
        acc *= 1099511628211ULL;
    }

    return acc;
}