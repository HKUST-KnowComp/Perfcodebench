#include "interface.h"
#include "xxhash.h"
#include <cstring>
#include <string>

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* data = input.data();
    size_t size = input.size();
    size_t pos = 0;
    while (pos < size) {
        const char* nl = static_cast<const char*>(std::memchr(data + pos, '\n', size - pos));
        size_t line_len;
        if (nl) {
            line_len = nl - (data + pos);
        } else {
            line_len = size - pos;
        }
        acc ^= static_cast<uint64_t>(XXH64(data + pos, line_len, 0));
        acc *= 1099511628211ULL;
        pos += line_len + (nl ? 1 : 0);
    }
    return acc;
}