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
  const char* data = text.data();
  const size_t size = text.size();
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    size_t start = 0;
    
    while (start < size) {
      size_t len = 0;
      // Fast scan for newline
      const char* ptr = data + start;
      const char* end_ptr = data + size;
      while (ptr < end_ptr && *ptr != '\n') {
        ptr++;
        len++;
      }

      // Process line length and first character
      hash = mix(hash, (uint64_t)len);
      hash = mix(hash, len == 0 ? 0u : static_cast<unsigned char>(data[start]));

      // Move start to character after the newline
      start = (ptr - data) + 1;
    }

    // Handle trailing newline case: if text ends with '\n', there is one final empty line
    if (size > 0 && data[size - 1] == '\n') {
      hash = mix(hash, 0ULL);
      hash = mix(hash, 0u);
    }

    final_hash = hash;
  }
  return final_hash;
}