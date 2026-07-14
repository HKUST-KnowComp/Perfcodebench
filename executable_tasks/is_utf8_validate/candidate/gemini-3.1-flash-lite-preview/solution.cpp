#include "interface.h"
#include "is_utf8.h"

/**
 * The is_utf8 library provides highly optimized SIMD-based validation.
 * By replacing the manual scalar loop with the library's validation function,
 * we benefit from hardware-accelerated instruction sets (like SSE4.2, AVX2, or NEON)
 * which process multiple bytes per cycle.
 */

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  for (const auto& s : inputs) {
    // The is_utf8 function returns true if the input is valid UTF-8.
    // It is designed to handle various string lengths efficiently.
    if (is_utf8(s.data(), s.size())) {
      count++;
    }
  }
  return count;
}