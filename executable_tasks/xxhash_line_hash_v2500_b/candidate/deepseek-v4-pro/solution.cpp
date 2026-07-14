#include "interface.h"
#include "xxhash.h"
#include <string>

uint64_t hash_lines(const std::string& input) {
    static const uint64_t EMPTY_HASH = XXH64("", 0, 0);
    uint64_t acc = 1469598103934665603ULL;
    const size_t total_len = input.size();
    const char* data = input.data();
    size_t start = 0;
    while (start <= total_len) {
        size_t end = input.find('\n', start);
        bool has_newline = (end != std::string::npos);
        size_t line_len = has_newline ? (end - start) : (total_len - start);
        uint64_t hash = (line_len == 0) ? EMPTY_HASH : XXH64(data + start, line_len, 0);
        acc ^= hash;
        acc *= 1099511628211ULL;
        if (!has_newline) break;
        start = end + 1;
    }
    return acc;
}
