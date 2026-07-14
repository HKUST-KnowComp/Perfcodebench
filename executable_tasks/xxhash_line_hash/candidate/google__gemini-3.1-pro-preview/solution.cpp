#include "interface.h"
#include "xxhash.h"
#include <cstring>
#include <string>

/**
 * Optimized line-wise xxHash64 batching.
 * 
 * Performance improvements:
 * 1. Removed std::string::substr(): This avoids heap allocations and data copying for every line.
 * 2. Used std::memchr(): This is a highly optimized SIMD-accelerated function for finding characters in memory.
 * 3. Direct pointer arithmetic: Minimizes indexing overhead.
 */
uint64_t hash_lines(const std::string& input) {
    const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    
    uint64_t acc = FNV_OFFSET_BASIS;
    const char* const base_ptr = input.data();
    const size_t total_size = input.size();
    size_t current_pos = 0;

    while (current_pos < total_size) {
        const char* const current_ptr = base_ptr + current_pos;
        const size_t remaining = total_size - current_pos;
        
        // Use memchr to find the next newline character efficiently.
        const char* const next_newline = static_cast<const char*>(std::memchr(current_ptr, '\n', remaining));

        size_t line_len;
        if (next_newline) {
            line_len = static_cast<size_t>(next_newline - current_ptr);
            // Move current_pos past the newline for the next iteration.
            current_pos += line_len + 1;
        } else {
            // No more newlines; process the remainder of the string as the last line.
            line_len = remaining;
            current_pos = total_size;
        }

        // Calculate XXH64 for the current line segment and update the accumulator.
        acc ^= XXH64(current_ptr, line_len, 0);
        acc *= FNV_PRIME;
    }

    return acc;
}
