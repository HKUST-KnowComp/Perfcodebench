#include "interface.h"
#include "xxhash.h"
#include <string>

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* data = input.data();
    size_t len = input.size();
    const char* start = data;
    const char* const end = data + len;

    while (start < end) {
        const char* line_end = start;
        while (line_end < end && *line_end != '\n') {
            ++line_end;
        }
        size_t line_len = line_end - start;
        acc ^= static_cast<uint64_t>(XXH64(start, line_len, 0));
        acc *= 1099511628211ULL;
        start = line_end + 1;
    }
    return acc;
}
