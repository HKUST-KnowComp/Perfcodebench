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
      size_t len = 0;
      // Manual scan for newline to avoid repeated find() overhead and substr()
      while (start + len < size && data[start + len] != '\n') {
        len++;
      }
      
      // Process the line immediately without storing it
      hash = mix(hash, static_cast<uint64_t>(len));
      hash = mix(hash, len == 0 ? 0u : static_cast<unsigned char>(data[start]));
      
      start += len + 1;
    }
    
    // Handle the case where the text ends with a newline (trailing empty line)
    if (size > 0 && data[size - 1] == '\n') {
      hash = mix(hash, 0ULL);
      hash = mix(hash, 0ULL);
    }
    
    final_hash = hash;
  }
  return final_hash;
}