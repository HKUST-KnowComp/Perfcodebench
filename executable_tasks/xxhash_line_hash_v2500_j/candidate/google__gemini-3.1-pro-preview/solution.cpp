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
 * 2. Replaced manual character-by-character scanning with std::memchr, which is typically 
 *    implemented using SIMD instructions (AVX2/SSE) for high-throughput searching.
 * 3. Minimized overhead in the accumulation loop by processing the input buffer directly.
 */
uint64_t hash_lines(const std::string& input) {
    // FNV-1a 64-bit constants
    const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;

    uint64_t acc = FNV_OFFSET_BASIS;
    const char* const data = input.data();
    const size_t total_size = input.size();
    size_t pos = 0;

    while (pos < total_size) {
        const char* current_ptr = data + pos;
        const size_t remaining = total_size - pos;

        // Use memchr to find the next newline character efficiently.
        // memchr is highly optimized in most standard libraries.
        const char* next_nl = static_cast<const char*>(std::memchr(current_ptr, '\n', remaining));

        size_t line_len;
        if (next_nl) {
            line_len = static_cast<size_t>(next_nl - current_ptr);
            // Move pos past the newline character for the next iteration.
            pos += line_len + 1;
        } else {
            // No more newlines; the rest of the buffer is the final line.
            line_len = remaining;
            pos = total_size;
        }

        // Calculate XXH64 for the current line segment.
        // Seed is 0 as per baseline requirement.
        uint64_t line_hash = XXH64(current_ptr, line_len, 0);

        // Combine using the FNV-1a style accumulation logic from the baseline.
        acc ^= line_hash;
        acc *= FNV_PRIME;
    }

    return acc;
}
