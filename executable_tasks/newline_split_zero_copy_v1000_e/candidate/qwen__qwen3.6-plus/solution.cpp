#include "interface.h"
#include <string>
#include <cstdint>
#include <cstring>

uint64_t newline_split_checksum(const std::string& text, int iters) {
  uint64_t hash = 0;
  const char* data = text.data();
  const char* const end_ptr = data + text.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    const char* p = data;
    while (p < end_ptr) {
      const char* nl = static_cast<const char*>(std::memchr(p, '\n', static_cast<std::size_t>(end_ptr - p)));
      std::size_t len = nl ? static_cast<std::size_t>(nl - p) : static_cast<std::size_t>(end_ptr - p);
      
      hash ^= len;
      hash *= 1099511628211ULL;
      
      hash ^= (len == 0 ? 0u : static_cast<unsigned char>(p[0]));
      hash *= 1099511628211ULL;
      
      p = nl ? nl + 1 : end_ptr;
    }
  }
  return hash;
}
