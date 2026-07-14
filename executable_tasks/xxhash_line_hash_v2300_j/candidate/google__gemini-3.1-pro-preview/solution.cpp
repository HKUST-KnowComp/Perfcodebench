#include "interface.h"
#include "xxhash.h"
#include <cstring>
#include <cstdint>
#include <string>

/**
 * Optimized line-wise xxHash64 batching.
 * 
 * Performance improvements:
 * 1. Replaced std::string::substr with direct pointer arithmetic to avoid memory allocations and copies.
 * 2. Replaced manual character-by-character scanning for '\n' with std::memchr, which is typically 
 *    implemented using SIMD instructions for high-speed searching.
 * 3. Minimized loop overhead and ensured zero-copy access to the input buffer.
 */
uint64_t hash_lines(const std::string& input) {
    // FNV-1a 64-bit offset basis
    uint64_t acc = 1469598103934665603ULL;
    // FNV-1a 64-bit prime
    const uint64_t fnv_prime = 1099511628211ULL;

    const char* curr = input.data();
    const size_t size = input.size();
    const char* end = curr + size;

    while (curr < end) {
        // Find the next newline character using optimized memchr
        const char* next_nl = static_cast<const char*>(std::memchr(curr, '\n', end - curr));
        
        size_t line_len;
        if (next_nl) {
            line_len = next_nl - curr;
        } else {
            // Last segment if no trailing newline
            line_len = end - curr;
        }

        // Hash the line segment directly from the input buffer
        // XXH64 is used with seed 0 as per the baseline requirement
        acc ^= static_cast<uint64_t>(XXH64(curr, line_len, 0));
        acc *= fnv_prime;

        if (next_nl) {
            // Move pointer past the newline character
            curr = next_nl + 1;
        } else {
            // No more newlines, exit loop
            break;
        }
    }

    return acc;
}
