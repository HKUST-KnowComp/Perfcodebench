#include "interface.h"

#include <string>

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
    
    const char* start = data;
    const char* end = data + size;
    
    while (start < end) {
      const char* line_end = start;
      while (line_end < end && *line_end != '\n') {
        ++line_end;
      }
      
      std::size_t line_len = line_end - start;
      hash = mix(hash, line_len);
      hash = mix(hash, line_len == 0 ? 0u : static_cast<unsigned char>(*start));
      
      start = line_end + 1;
    }
  }
  
  return hash;
}
