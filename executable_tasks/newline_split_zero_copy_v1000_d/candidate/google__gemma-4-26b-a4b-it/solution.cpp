#include "interface.h"
#include <string>
#include <string_view>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  const char* data = text.data();
  const size_t n = text.size();
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    size_t start = 0;
    
    while (start < n) {
      size_t end = start;
      // Manual scan for newline is often faster than std::string::find for large buffers
      // due to better compiler vectorization/SIMD opportunities.
      while (end < n && data[end] != '\n') {
        end++;
      }

      size_t line_len = end - start;
      // First character logic: if line is empty, use 0, else use first char
      uint64_t first_char = (line_len == 0) ? 0 : static_cast<unsigned char>(data[start]);
      
      hash = mix(hash, line_len);
      hash = mix(hash, first_char);

      start = end + 1;
    }
    final_hash = hash;
  }

  return final_hash;
}
