#include "interface.h"
#include "simdutf.h"

/**
 * Optimized implementation using the simdutf library.
 * simdutf provides hardware-accelerated UTF-8 validation using SIMD instructions
 * (SSE4.2, AVX2, AVX-512, NEON, etc.), which is significantly faster than
 * manual scalar byte-by-byte validation.
 */
std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  for (const auto& s : inputs) {
    if (simdutf::validate_utf8(s.data(), s.size())) {
      count++;
    }
  }
  return count;
}