#include "interface.h"
#include "simdutf.h"

/**
 * count_valid_strings
 * 
 * This implementation uses the simdutf library to validate UTF-8 strings.
 * simdutf provides highly optimized SIMD implementations (AVX2, SSE4.1, NEON, etc.)
 * for UTF-8 validation, which is significantly faster than scalar bit-masking loops.
 */
std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  for (const auto& s : inputs) {
    // simdutf::validate_utf8 returns true if the buffer is valid UTF-8.
    // It automatically selects the best SIMD implementation for the current CPU.
    if (simdutf::validate_utf8(s.data(), s.size())) {
      count++;
    }
  }
  return count;
}