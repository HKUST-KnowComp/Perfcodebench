#include "interface.h"

#include <cstdint>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  const char* data = text.data();
  const std::size_t size = text.size();
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    const char* ptr = data;
    const char* end = data + size;
    
    while (ptr < end) {
      const char* line_start = ptr;
      
      // Find newline or end of buffer
      while (ptr < end && *ptr != '\n') {
        ++ptr;
      }
      
      std::size_t line_len = ptr - line_start;
      
      // Mix line length
      hash = mix(hash, line_len);
      
      // Mix first character (0 if empty line)
      hash = mix(hash, line_len == 0 ? 0u : static_cast<unsigned char>(*line_start));
      
      // Skip the newline if present
      if (ptr < end) {
        ++ptr;
      }
    }
  }
  
  return hash;
}