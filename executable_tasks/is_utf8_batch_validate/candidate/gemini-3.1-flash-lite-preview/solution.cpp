#include "interface.h"
#include "is_utf8.h"

/**
 * Optimized implementation using the provided is_utf8 library.
 * The library function is_utf8(const char*, size_t) is typically implemented
 * using SIMD instructions (like SSE4.2 or AVX2) to validate UTF-8 sequences
 * much faster than a manual byte-by-byte scalar loop.
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