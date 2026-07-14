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
  uint64_t final_hash = 0;
  const char* data = text.data();
  const size_t size = text.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    size_t start = 0;
    
    while (start < size) {
      size_t end = start;
      // Manual scan for newline to avoid overhead of std::string::find
      while (end < size && data[end] != '\n') {
        ++end;
      }
      
      size_t len = end - start;
      hash = mix(hash, (uint64_t)len);
      hash = mix(hash, len == 0 ? 0u : static_cast<unsigned char>(data[start]));
      
      start = end + 1;
    }
    
    // Handle trailing newline case: if text ends with '\n', there is one more empty line
    if (size > 0 && data[size - 1] == '\n') {
      hash = mix(hash, 0u);
      hash = mix(hash, 0u);
    }
    
    final_hash = hash;
  }
  
  // The original code logic: if text is empty, the loop doesn't run, 
  // but the hash is reset to 1469598103934665603ULL inside the iters loop.
  // If text is empty, the original code results in hash = 1469598103934665603ULL.
  if (size == 0) return 1469598103934665603ULL;

  return final_hash;
}