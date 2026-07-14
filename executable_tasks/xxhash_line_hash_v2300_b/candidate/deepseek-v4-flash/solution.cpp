#include "interface.h"
#include "xxhash.h"
#include <cstring>
#include <string>

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* data = input.data();
    size_t len = input.size();
    size_t start = 0;
    while (start < len) {
        const char* newline_pos = (const char*)std::memchr(data + start, '\n', len - start);
        size_t end;
        if (newline_pos == nullptr) {
            end = len;
        } else {
            end = newline_pos - data;
        }
        size_t line_len = end - start;
        uint64_t hash = XXH64(data + start, line_len, 0);
        acc ^= hash;
        acc *= 1099511628211ULL;
        start = end + 1;
    }
    return acc;
}