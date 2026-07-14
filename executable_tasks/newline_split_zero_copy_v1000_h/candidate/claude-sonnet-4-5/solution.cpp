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
  uint64_t hash = 0;
  const char* data = text.data();
  const std::size_t size = text.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    std::size_t start = 0;
    while (start < size) {
      const char* line_start = data + start;
      const char* p = line_start;
      const char* end = data + size;
      
      while (p < end && *p != '\n') {
        ++p;
      }
      
      std::size_t line_len = p - line_start;
      hash = mix(hash, line_len);
      hash = mix(hash, line_len == 0 ? 0u : static_cast<unsigned char>(*line_start));
      
      start = (p - data) + 1;
    }
  }
  
  return hash;
}