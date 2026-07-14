#include "interface.h"
#include "xxhash.h"
#include <string>
#include <cstdint>

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* data = input.data();
    const char* end_ptr = data + input.size();
    const char* current = data;

    while (current < end_ptr) {
        const char* line_start = current;
        // Fast scan for newline
        while (current < end_ptr && *current != '\n') {
            ++current;
        }
        
        size_t line_len = static_cast<size_t>(current - line_start);
        
        // Use XXH64 directly on the memory range to avoid std::string allocation
        acc ^= XXH64(line_start, line_len, 0);
        acc *= 1099511628211ULL;

        // Skip the newline character itself
        if (current < end_ptr && *current == '\n') {
            ++current;
        }
    }

    return acc;
}