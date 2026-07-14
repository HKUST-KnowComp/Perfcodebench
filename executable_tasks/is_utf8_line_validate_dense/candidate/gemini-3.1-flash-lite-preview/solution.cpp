#include "interface.h"
#include "is_utf8.h"

/**
 * Optimized implementation using the provided is_utf8 library.
 * The library provides highly optimized SIMD-based validation routines.
 */
std::size_t count_valid_lines(const std::string& input) {
  std::size_t count = 0;
  const char* p = input.data();
  const char* const end = p + input.size();

  while (p < end) {
    const char* line_end = static_cast<const char*>(memchr(p, '\n', end - p));
    const char* next_start = (line_end != nullptr) ? (line_end + 1) : end;
    std::size_t len = (line_end != nullptr) ? (static_cast<std::size_t>(line_end - p)) : (static_cast<std::size_t>(end - p));

    // Use the high-performance SIMD validator from is_utf8.h
    if (is_utf8(reinterpret_cast<const unsigned char*>(p), len)) {
      count++;
    }

    p = next_start;
  }
  return count;
}