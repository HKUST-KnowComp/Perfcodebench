#include "interface.h"

#include "xxhash.h"

#include <cstdint>
#include <string>

uint64_t hash_lines(const std::string& input) {
    const uint64_t initial = 1469598103934665603ULL;
    const uint64_t multiplier = 1099511628211ULL;
    uint64_t acc = initial;
    const char* data = input.data();
    size_t len = input.size();
    size_t i = 0;
    while (i < len) {
        size_t j = i;
        while (j < len && data[j] != '\n') {
            ++j;
        }
        size_t line_len = j - i;
        acc ^= static_cast<uint64_t>(XXH64(data + i, line_len, 0));
        acc *= multiplier;
        i = j + 1;
    }
    return acc;
}