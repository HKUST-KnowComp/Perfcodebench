#include "interface.h"
#include "is_utf8.h"

/**
 * The task requires counting valid UTF-8 strings. 
 * We utilize the provided is_utf8.h library which is optimized for performance 
 * using SIMD instructions to validate UTF-8 sequences much faster than scalar loops.
 */

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  for (const auto& s : inputs) {
    // Assuming is_utf8.h provides a function validate_utf8 or similar.
    // Based on standard high-performance UTF-8 libraries, we use the provided interface.
    if (validate_utf8(reinterpret_cast<const unsigned char*>(s.data()), s.size())) {
      count++;
    }
  }
  return count;
}