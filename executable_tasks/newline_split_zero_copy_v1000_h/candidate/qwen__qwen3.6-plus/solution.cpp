#include "interface.h"
#include <cstring>
#include <string>

uint64_t newline_split_checksum(const std::string& text, int iters) {
  uint64_t hash = 0;
  const char* data = text.data();
  const char* const end_ptr = data + text.size();
  constexpr uint64_t FNV_PRIME = 1099511628211ULL;
  constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    hash = FNV_OFFSET;
    const char* p = data;
    while (p < end_ptr) {
      const char* nl = static_cast<const char*>(std::memchr(p, '\n', end_ptr - p));
      std::size_t line_len = nl ? (nl - p) : (end_ptr - p);
      
      hash ^= line_len;
      hash *= FNV_PRIME;
      
      hash ^= (line_len > 0) ? static_cast<unsigned char>(p[0]) : 0u;
      hash *= FNV_PRIME;
      
      p = nl ? nl + 1 : end_ptr;
    }
  }
  return hash;
}
