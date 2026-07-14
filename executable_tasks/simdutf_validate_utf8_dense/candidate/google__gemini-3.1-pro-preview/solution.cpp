#include "interface.h"
#include "simdutf.h"

/**
 * Optimized UTF-8 validation using the simdutf library.
 * 
 * The task involves validating a vector of strings, which are described as a 
 * "mostly-valid long-string workload". The simdutf library is specifically 
 * designed for this use case, utilizing SIMD instructions (AVX2, NEON, etc.) 
 * to validate UTF-8 sequences much faster than a scalar state machine.
 */
std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
    std::size_t count = 0;
    for (const auto& s : inputs) {
        // simdutf::validate_utf8 returns true if the buffer is valid UTF-8.
        // It handles the complexity of multi-byte sequences and surrogate pairs
        // using SIMD-parallel bit manipulation and lookup tables.
        if (simdutf::validate_utf8(s.data(), s.size())) {
            count++;
        }
    }
    return count;
}