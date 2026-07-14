#include "interface.h"
#include "is_utf8.h"

/**
 * Optimized implementation using the provided is_utf8 library.
 * The library is designed to use SIMD instructions for high-performance
 * validation of UTF-8 strings, which is significantly faster than
 * manual scalar byte-by-byte checking.
 */
std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  for (const auto& s : inputs) {
    // is_utf8 is expected to be a high-performance implementation
    // provided by the environment.
    if (is_utf8(s.data(), s.size())) {
      count++;
    }
  }
  return count;
}