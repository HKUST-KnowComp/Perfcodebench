#include "interface.h"
#include "xxhash.h"
#include <cstring>
#include <string>

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* data = input.data();
    std::size_t size = input.size();
    std::size_t start = 0;
    const uint64_t multiplier = 1099511628211ULL;

    while (start < size) {
        const char* line_start = data + start;
        const void* nl = std::memchr(line_start, '\n', size - start);
        std::size_t line_len;
        if (nl != nullptr) {
            const char* line_end = static_cast<const char*>(nl);
            line_len = line_end - line_start;
            start = line_end - data + 1;
        } else {
            line_len = size - start;
            start = size;
        }
        acc ^= static_cast<uint64_t>(XXH64(line_start, line_len, 0));
        acc *= multiplier;
    }
    return acc;
}