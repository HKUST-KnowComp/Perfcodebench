#include "interface.h"
#include "xxhash.h"
#include <string>
#include <cstring>

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* data = input.data();
    const char* end_ptr = data + input.size();
    const char* current = data;

    while (current < end_ptr) {
        const char* line_start = current;
        // Use memchr for highly optimized newline scanning
        const char* next_newline = static_cast<const char*>(std::memchr(current, '\n', end_ptr - current));
        
        if (next_newline) {
            size_t line_len = next_newline - line_start;
            acc ^= XXH64(line_start, line_len, 0);
            acc *= 1099511628211ULL;
            current = next_newline + 1;
        } else {
            // Last line without a trailing newline
            size_t line_len = end_ptr - line_start;
            acc ^= XXH64(line_start, line_len, 0);
            acc *= 1099511628211ULL;
            current = end_ptr;
        }
    }

    return acc;
}