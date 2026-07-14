#include "interface.h"

#include <string>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
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
      std::size_t end = start;
      while (end < size && data[end] != '\n') {
        ++end;
      }
      
      std::size_t len = end - start;
      hash = mix(hash, len);
      hash = mix(hash, len == 0 ? 0u : static_cast<unsigned char>(data[start]));
      
      start = end + 1;
    }
  }
  
  return hash;
}
