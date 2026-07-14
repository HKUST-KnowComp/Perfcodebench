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
      // Manual scan for newline to avoid std::string::find overhead and potential bounds checks
      while (end < size && data[end] != '\n') {
        ++end;
      }

      size_t line_len = end - start;
      hash = mix(hash, (uint64_t)line_len);
      hash = mix(hash, line_len == 0 ? 0u : (unsigned char)data[start]);

      start = end + 1;
    }

    // Handle the case where the text ends with a newline (trailing empty line)
    if (start == size && size > 0 && data[size - 1] == '\n') {
      hash = mix(hash, 0ULL);
      hash = mix(hash, 0u);
    }
    
    final_hash = hash;
  }
  return final_hash;
}
