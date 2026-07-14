#include "interface.h"
#include "xxhash.h"
#include <cstdint>
#include <string>

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* p = input.data();
    const char* const end = p + input.size();

    while (p < end) {
        const char* line_start = p;
        while (p < end && *p != '\n') {
            ++p;
        }
        std::size_t line_len = static_cast<std::size_t>(p - line_start);
        acc ^= static_cast<uint64_t>(XXH64(line_start, line_len, 0));
        acc *= 1099511628211ULL;
        if (p < end) {
            ++p; // skip newline
        }
    }
    return acc;
}
