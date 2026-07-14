#include "interface.h"
#include "is_utf8.h"

/**
 * Optimized implementation using the provided is_utf8 library.
 * The library is designed to use SIMD instructions (SSE/AVX/NEON) 
 * to validate UTF-8 sequences much faster than manual scalar loops.
 */
std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  for (const auto& s : inputs) {
    // The is_utf8 function is highly optimized for performance.
    // It returns true if the string is valid UTF-8.
    if (is_utf8(s.data(), s.size())) {
      count++;
    }
  }
  return count;
}