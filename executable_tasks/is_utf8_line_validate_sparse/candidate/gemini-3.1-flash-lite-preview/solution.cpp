#include "interface.h"
#include "is_utf8.h"

/**
 * Optimized implementation using the provided is_utf8 library.
 * The library provides highly optimized SIMD-based validation.
 * We iterate through the string to identify line boundaries and 
 * validate each segment using the library's function.
 */
std::size_t count_valid_lines(const std::string& input) {
  std::size_t count = 0;
  const char* p = input.data();
  const char* const end = p + input.size();

  while (p < end) {
    const char* line_end = static_cast<const char*>(std::memchr(p, '\n', end - p));
    const std::size_t len = (line_end != nullptr) ? static_cast<std::size_t>(line_end - p) : static_cast<std::size_t>(end - p);

    // Use the optimized library function for validation
    if (is_utf8(reinterpret_cast<const unsigned char*>(p), len)) {
      count++;
    }

    if (line_end == nullptr) {
      break;
    }
    p = line_end + 1;
  }

  return count;
}