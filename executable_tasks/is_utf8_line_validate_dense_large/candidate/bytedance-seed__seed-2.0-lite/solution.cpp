#include "interface.h"
#include "is_utf8.h"
#include <cstring>

std::size_t count_valid_lines(const std::string& input) {
  std::size_t count = 0;
  const char* p = input.data();
  const char* const end = p + input.size();
  
  while (p < end) {
    // Use vectorized memchr to find next newline, far faster than manual scalar scan
    const char* line_end = static_cast<const char*>(std::memchr(p, '\n', end - p));
    if (!line_end) {
      line_end = end;
    }
    
    const std::size_t line_len = static_cast<std::size_t>(line_end - p);
    // Use SIMD-accelerated UTF-8 validation from is_utf8.h
    if (simdutf::validate_utf8(p, line_len)) {
      ++count;
    }
    
    // Advance past newline to start of next line, identical to original logic
    p = (line_end < end) ? (line_end + 1) : line_end;
  }
  return count;
}