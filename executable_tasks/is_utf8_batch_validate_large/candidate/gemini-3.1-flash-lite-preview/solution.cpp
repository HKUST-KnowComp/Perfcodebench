#include "interface.h"
#include "is_utf8.h"

/**
 * Optimized implementation using the provided is_utf8 library.
 * The library is designed to handle UTF-8 validation efficiently,
 * likely utilizing SIMD instructions to process multiple bytes per cycle.
 */
std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  for (const auto& s : inputs) {
    // is_utf8 is expected to be a high-performance validation function
    // provided by the environment's is_utf8.h header.
    if (is_utf8(s.data(), s.size())) {
      count++;
    }
  }
  return count;
}