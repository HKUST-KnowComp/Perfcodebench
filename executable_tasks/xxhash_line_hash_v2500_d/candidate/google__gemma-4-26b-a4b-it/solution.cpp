#include "interface.h"
#include "xxhash.h"
#include <string>
#include <cstring>

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* ptr = input.data();
    const char* end_ptr = ptr + input.size();

    while (ptr < end_ptr) {
        const char* line_start = ptr;
        // Fast scan for newline
        while (ptr < end_ptr && *ptr != '\n') {
            ++ptr;
        }
        
        size_t line_len = static_cast<size_t>(ptr - line_start);
        
        // Use XXH64 directly on the buffer segment to avoid allocations
        acc ^= XXH64(line_start, line_len, 0);
        acc *= 1099511628211ULL;

        // Skip the newline character itself
        if (ptr < end_ptr && *ptr == '\n') {
            ++ptr;
        }
    }
    return acc;
}