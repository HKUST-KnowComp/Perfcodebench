#include "interface.h"
#include "xxhash.h"
#include <string>
#include <cstring>

/**
 * Optimized implementation of line-wise xxHash64 batching.
 * 
 * Improvements:
 * 1. Replaced character-by-character scanning with `memchr`, which is typically 
 *    implemented using SIMD instructions for significantly faster search.
 * 2. Eliminated `std::string::substr` calls which performed heap allocations 
 *    and memory copies for every single line.
 * 3. Directly passed pointers and lengths to `XXH64` to minimize overhead.
 */
uint64_t hash_lines(const std::string& input) {
    const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    
    uint64_t acc = FNV_OFFSET_BASIS;
    const char* ptr = input.data();
    const size_t total_size = input.size();
    const char* const end_ptr = ptr + total_size;

    while (ptr < end_ptr) {
        // Use memchr to find the next newline character efficiently
        const char* next_nl = static_cast<const char*>(std::memchr(ptr, '\n', end_ptr - ptr));
        
        size_t line_len;
        const char* next_start;
        
        if (next_nl) {
            line_len = next_nl - ptr;
            next_start = next_nl + 1;
        } else {
            // Handle the last segment if it doesn't end with a newline
            line_len = end_ptr - ptr;
            next_start = end_ptr;
        }

        // Calculate XXH64 for the current line segment
        uint64_t h = XXH64(ptr, line_len, 0);
        
        // Combine using the FNV-1a style logic provided in the baseline
        acc ^= h;
        acc *= FNV_PRIME;

        ptr = next_start;
    }

    return acc;
}