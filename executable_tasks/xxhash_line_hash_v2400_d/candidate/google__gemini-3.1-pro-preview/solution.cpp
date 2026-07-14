#include "interface.h"
#include "xxhash.h"
#include <string>
#include <cstring>

/**
 * Optimized line-wise xxHash64 implementation.
 * 
 * Performance improvements:
 * 1. Removed std::string::substr(): The baseline created a new string object for every line,
 *    triggering memory allocations and data copies. We now pass raw pointers directly to XXH64.
 * 2. Replaced manual character scanning: Used std::memchr() to find newlines. 
 *    Modern implementations of memchr use SIMD instructions (AVX2/SSE2) to scan memory 
 *    significantly faster than a byte-by-byte loop.
 * 3. Direct pointer arithmetic: Minimized overhead by working with raw pointers and 
 *    pre-calculating remaining lengths.
 */
uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL; // FNV-1a 64-bit offset basis
    const uint64_t fnv_prime = 1099511628211ULL; // FNV-1a 64-bit prime
    
    const char* ptr = input.data();
    const size_t total_size = input.size();
    size_t processed = 0;

    while (processed < total_size) {
        const char* current_start = ptr + processed;
        size_t remaining = total_size - processed;
        
        // Use memchr for SIMD-accelerated newline searching
        const char* next_nl = static_cast<const char*>(std::memchr(current_start, '\n', remaining));
        
        size_t line_len;
        if (next_nl) {
            line_len = next_nl - current_start;
            processed += line_len + 1; // Skip the newline character
        } else {
            line_len = remaining;
            processed = total_size; // End of input
        }

        // Calculate XXH64 for the line segment directly from the input buffer
        uint64_t h = XXH64(current_start, line_len, 0);
        
        // Combine using the FNV-1a style logic required by the task
        acc ^= h;
        acc *= fnv_prime;
    }
    
    return acc;
}