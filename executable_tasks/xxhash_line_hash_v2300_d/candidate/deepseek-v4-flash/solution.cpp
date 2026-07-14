#include "interface.h"
#include "xxhash.h"
#include <cstring>
#include <string>

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* data = input.data();
    size_t len = input.size();
    const char* pos = data;
    const char* end = data + len;
    while (pos < end) {
        const char* nl = static_cast<const char*>(memchr(pos, '\n', end - pos));
        size_t line_len;
        if (nl) {
            line_len = nl - pos;
        } else {
            line_len = end - pos;
        }
        acc ^= static_cast<uint64_t>(XXH64(pos, line_len, 0));
        acc *= 1099511628211ULL;
        if (nl) {
            pos = nl + 1;
        } else {
            break;
        }
    }
    return acc;
}