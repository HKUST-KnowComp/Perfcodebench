#include "interface.h"
#include "is_utf8.h"
#include <cstddef>
#include <string>
#include <vector>

/**
 * Performance Engineer's Optimization:
 * The task involves validating a batch of longer UTF-8 strings where invalid records are sparse.
 * The baseline scalar implementation is branch-heavy and processes one byte at a time.
 * By utilizing the 'is_utf8' library (likely based on SIMD algorithms like Lemire's or similar),
 * we can validate large chunks of data using vectorized instructions (SSE/AVX).
 * This is particularly effective for 'longer' strings as the SIMD setup overhead is amortized.
 */

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
    std::size_t valid_count = 0;
    
    // Iterate through the batch and use the optimized library function.
    // is_utf8 typically provides a fast path for ASCII and uses SIMD for multi-byte sequences.
    for (const auto& s : inputs) {
        if (is_utf8(s.data(), s.size())) {
            valid_count++;
        }
    }
    
    return valid_count;
}