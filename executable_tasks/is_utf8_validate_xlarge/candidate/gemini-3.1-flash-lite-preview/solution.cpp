#include "interface.h"
#include "is_utf8.h"

/**
 * The is_utf8 library provides highly optimized SIMD-based validation.
 * By utilizing the provided library, we benefit from architecture-specific
 * optimizations (SSE4.2, AVX2, NEON) that outperform manual scalar loops.
 */

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  for (const auto& s : inputs) {
    if (is_utf8(s.data(), s.size())) {
      count++;
    }
  }
  return count;
}