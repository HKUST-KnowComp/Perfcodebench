#include "interface.h"
#include "simdjson.h"

/**
 * Optimized UTF-8 validation using simdjson.
 * 
 * simdjson provides highly optimized SIMD kernels (AVX2, SSE4.2, NEON) 
 * for UTF-8 validation. This implementation iterates through the input 
 * strings and uses the library's built-in validation function.
 */
std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
    std::size_t count = 0;
    
    // We use the global/static implementation selector in simdjson.
    // validate_utf8 is a fast SIMD-based check.
    for (const auto& s : inputs) {
        if (simdjson::validate_utf8(s.data(), s.size())) {
            count++;
        }
    }
    
    return count;
}