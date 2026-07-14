#include "interface.h"
#include <string>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  const size_t n = text.size();
  const char* data = text.data();
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    size_t start = 0;

    while (start < n) {
      size_t end = 0;
      // Use find to leverage SIMD-optimized implementations in the STL
      size_t pos = text.find('\n', start);
      
      if (pos == std::string::npos) {
        end = n;
      } else {
        end = pos;
      }

      size_t line_len = end - start;
      
      // Update hash with line length
      hash = mix(hash, line_len);
      
      // Update hash with first character (if not empty)
      if (line_len > 0) {
        hash = mix(hash, static_cast<unsigned char>(data[start]));
      } else {
        hash = mix(hash, 0u);
      }

      if (pos == std::string::npos) {
        start = n;
      } else {
        start = pos + 1;
      }
    }
    final_hash = hash;
  }

  return final_hash;
}
