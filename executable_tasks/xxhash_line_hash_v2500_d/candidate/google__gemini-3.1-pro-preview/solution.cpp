#include "interface.h"
#include "xxhash.h"
#include <string>
#include <cstring>

/**
 * Optimized line-wise xxHash64 batching.
 * 
 * Performance improvements:
 * 1. Replaced character-by-character scan with 'memchr', which uses SIMD instructions internally.
 * 2. Eliminated 'std::string::substr' which performs heap allocations and data copying.
 * 3. Direct pointer arithmetic to pass line segments to XXH64.
 * 4. Maintained the exact FNV-1a accumulation logic required by the task.
 */
uint64_t hash_lines(const std::string& input) {
    const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    
    uint64_t acc = FNV_OFFSET_BASIS;
    const char* ptr = input.data();
    const size_t total_size = input.size();
    const char* const end_ptr = ptr + total_size;

    while (ptr < end_ptr) {
        // Find the next newline character using optimized memchr
        const void* next_nl = std::memchr(ptr, '\n', static_cast<size_t>(end_ptr - ptr));
        
        size_t line_len;
        const char* next_start;
        
        if (next_nl) {
            line_len = static_cast<const char*>(next_nl) - ptr;
            next_start = static_cast<const char*>(next_nl) + 1;
        } else {
            // Handle the last segment if it doesn't end with a newline
            line_len = static_cast<size_t>(end_ptr - ptr);
            next_start = end_ptr;
        }

        // Calculate XXH64 for the current line segment
        uint64_t h = XXH64(ptr, line_len, 0);
        
        // FNV-1a style accumulation
        acc ^= h;
        acc *= FNV_PRIME;

        ptr = next_start;
    }

    return acc;
}
