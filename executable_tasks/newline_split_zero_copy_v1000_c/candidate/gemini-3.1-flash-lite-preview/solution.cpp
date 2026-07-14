#include "interface.h"
#include <string>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  uint64_t final_hash = 0;
  const char* data = text.data();
  const size_t size = text.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    size_t start = 0;
    
    while (start < size) {
      const char* ptr = data + start;
      const char* next_nl = static_cast<const char*>(__builtin_memchr(ptr, '\n', size - start));
      
      size_t end = (next_nl == nullptr) ? size : (next_nl - data);
      size_t len = end - start;
      
      hash = mix(hash, len);
      hash = mix(hash, (len == 0) ? 0u : static_cast<unsigned char>(data[start]));
      
      start = end + 1;
    }
    final_hash = hash;
  }
  return final_hash;
}