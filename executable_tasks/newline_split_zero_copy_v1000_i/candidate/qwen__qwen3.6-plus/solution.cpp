#include "interface.h"

#include <string>

uint64_t newline_split_checksum(const std::string& text, int iters) {
  uint64_t hash = 0;
  const char* data = text.data();
  size_t len = text.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    size_t start = 0;
    while (start < len) {
      size_t end = text.find('\n', start);
      if (end == std::string::npos) {
        end = len;
      }
      size_t line_len = end - start;
      uint64_t first_char = (line_len > 0) ? static_cast<unsigned char>(data[start]) : 0u;
      
      hash ^= line_len;
      hash *= 1099511628211ULL;
      hash ^= first_char;
      hash *= 1099511628211ULL;
      
      start = end + 1;
    }
  }
  return hash;
}
