#include "interface.h"
#include "simdutf.h"

/**
 * count_valid_strings
 * 
 * This implementation leverages the simdutf library, which provides
 * industry-leading performance for UTF-8 validation using SIMD instructions
 * (AVX2, SSE4.2, NEON, etc.). 
 */
std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  
  // Iterate through each string and use simdutf's optimized validation.
  // simdutf::validate_utf8 is designed to handle both short and long strings
  // efficiently by dispatching to the appropriate SIMD kernel at runtime.
  for (const auto& s : inputs) {
    if (simdutf::validate_utf8(s.data(), s.size())) {
      count++;
    }
  }
  
  return count;
}