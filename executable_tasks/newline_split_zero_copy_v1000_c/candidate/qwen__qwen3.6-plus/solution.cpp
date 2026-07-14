#include "interface.h"
#include <cstring>

uint64_t newline_split_checksum(const std::string& text, int iters) {
  uint64_t hash = 0;
  const char* data = text.data();
  const char* end = data + text.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    const char* p = data;
    while (p < end) {
      const char* nl = static_cast<const char*>(std::memchr(p, '\n', end - p));
      size_t line_len = (nl ? nl : end) - p;
      
      hash ^= line_len;
      hash *= 1099511628211ULL;
      
      hash ^= (line_len > 0) ? static_cast<unsigned char>(p[0]) : 0u;
      hash *= 1099511628211ULL;
      
      p = (nl ? nl : end) + 1;
    }
  }
  return hash;
}
