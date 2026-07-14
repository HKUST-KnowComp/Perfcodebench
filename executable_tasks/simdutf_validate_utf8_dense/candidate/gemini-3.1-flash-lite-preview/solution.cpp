#include "interface.h"
#include "simdutf.h"

/**
 * Optimized UTF-8 validation using the simdutf library.
 * simdutf automatically detects the best available SIMD instruction set 
 * (AVX2, AVX-512, NEON, etc.) to perform validation at high throughput.
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