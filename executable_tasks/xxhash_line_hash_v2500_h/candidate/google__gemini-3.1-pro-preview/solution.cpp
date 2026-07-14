#include "interface.h"
#include "xxhash.h"
#include <string>
#include <cstring>

/**
 * Optimized line-wise xxHash64 implementation.
 * 
 * Performance improvements:
 * 1. Replaced std::string::substr with direct pointer arithmetic to avoid memory allocations and copies.
 * 2. Used memchr() for newline detection, which is typically implemented with SIMD instructions.
 * 3. Minimized branch overhead in the main loop.
 */
uint64_t hash_lines(const std::string& input) {
    const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    
    uint64_t acc = FNV_OFFSET_BASIS;
    
    const char* ptr = input.data();
    const size_t size = input.size();
    const char* const end_ptr = ptr + size;

    while (ptr < end_ptr) {
        // Find the next newline character using optimized memchr
        const char* next_nl = static_cast<const char*>(std::memchr(ptr, '\n', end_ptr - ptr));
        
        size_t line_len;
        const char* next_start;
        
        if (next_nl) {
            line_len = static_cast<size_t>(next_nl - ptr);
            next_start = next_nl + 1;
        } else {
            // Handle the last segment if it doesn't end with a newline
            line_len = static_cast<size_t>(end_ptr - ptr);
            next_start = end_ptr;
        }

        // Calculate XXH64 for the current line segment
        // Seed is 0 as per baseline
        uint64_t h = XXH64(ptr, line_len, 0);

        // FNV-1a style accumulation
        acc ^= h;
        acc *= FNV_PRIME;

        ptr = next_start;
    }

    return acc;
}