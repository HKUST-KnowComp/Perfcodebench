#include "interface.h"

#include <cstdint>
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
  const size_t len = text.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    size_t start = 0;
    
    while (start < len) {
      size_t line_len = 0;
      const char* ptr = data + start;
      const char* end_ptr = data + len;
      
      // Fast scan for newline
      while (ptr < end_ptr && *ptr != '\n') {
        ptr++;
        line_len++;
      }
      
      // Mix length
      hash = mix(hash, static_cast<uint64_t>(line_len));
      // Mix first character
      hash = mix(hash, line_len == 0 ? 0u : static_cast<unsigned char>(data[start]));
      
      // Move start to character after newline
      start += line_len + 1;
    }
    
    // Handle trailing newline case: if the string ends with '\n', 
    // the loop above finishes, but a final empty line exists.
    if (len > 0 && data[len - 1] == '\n') {
      hash = mix(hash, 0ULL);
      hash = mix(hash, 0u);
    }

    final_hash = hash;
  }
  return final_hash;
}