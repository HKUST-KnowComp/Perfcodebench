#include "interface.h"
#include <cstring>
#include <string>

uint64_t newline_split_checksum(const std::string& text, int iters) {
  const char* data = text.data();
  const size_t len = text.size();
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    size_t start = 0;
    while (start < len) {
      const char* p = data + start;
      const char* nl = static_cast<const char*>(std::memchr(p, '\n', len - start));
      size_t end = nl ? (nl - data) : len;
      
      size_t line_len = end - start;
      uint64_t first_char = line_len ? static_cast<unsigned char>(data[start]) : 0u;
      
      hash ^= line_len;
      hash *= 1099511628211ULL;
      hash ^= first_char;
      hash *= 1099511628211ULL;
      
      start = end + 1;
    }
  }
  return hash;
}