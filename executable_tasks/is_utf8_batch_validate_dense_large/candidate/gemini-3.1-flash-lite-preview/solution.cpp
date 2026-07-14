#include "interface.h"
#include "is_utf8.h"

/**
 * Optimized implementation using the provided is_utf8 library.
 * The library is designed to utilize SIMD instructions (AVX2/NEON) 
 * to validate UTF-8 sequences much faster than manual scalar loops.
 */
std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  for (const auto& s : inputs) {
    // The is_utf8 function is expected to be highly optimized for performance.
    if (is_utf8(reinterpret_cast<const unsigned char*>(s.data()), s.size())) {
      count++;
    }
  }
  return count;
}