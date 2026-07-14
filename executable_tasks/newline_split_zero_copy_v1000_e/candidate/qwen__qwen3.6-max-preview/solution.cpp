#include "interface.h"
#include <string>
#include <cstdint>

uint64_t newline_split_checksum(const std::string& text, int iters) {
  uint64_t hash = 0;
  const char* data = text.data();
  size_t len = text.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    size_t line_start = 0;
    for (size_t i = 0; i < len; ++i) {
      if (data[i] == '\n') {
        size_t line_len = i - line_start;
        hash ^= line_len;
        hash *= 1099511628211ULL;
        hash ^= (line_len > 0 ? static_cast<unsigned char>(data[line_start]) : 0u);
        hash *= 1099511628211ULL;
        line_start = i + 1;
      }
    }
    if (line_start < len) {
      size_t line_len = len - line_start;
      hash ^= line_len;
      hash *= 1099511628211ULL;
      hash ^= static_cast<unsigned char>(data[line_start]);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
