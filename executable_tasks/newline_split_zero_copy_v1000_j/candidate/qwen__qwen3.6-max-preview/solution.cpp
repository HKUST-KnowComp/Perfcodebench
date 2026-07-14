#include "interface.h"
#include <string>
#include <cstdint>

uint64_t newline_split_checksum(const std::string& text, int iters) {
  uint64_t hash = 0;
  const char* data = text.data();
  std::size_t len = text.size();
  const char* end_ptr = data + len;

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    const char* p = data;
    while (p < end_ptr) {
      const char* line_start = p;
      while (p < end_ptr && *p != '\n') {
        ++p;
      }
      std::size_t line_len = static_cast<std::size_t>(p - line_start);
      
      hash ^= line_len;
      hash *= 1099511628211ULL;
      
      uint64_t first_char = (line_len == 0) ? 0ULL : static_cast<unsigned char>(*line_start);
      hash ^= first_char;
      hash *= 1099511628211ULL;
      
      if (p < end_ptr) ++p;
    }
  }
  return hash;
}
