#include "interface.h"

#include <string>
#include <cstdint>

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
  const char* end_ptr = data + text.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    const char* ptr = data;
    while (ptr < end_ptr) {
      const char* line_start = ptr;
      while (ptr < end_ptr && *ptr != '\n') {
        ++ptr;
      }
      std::size_t line_len = static_cast<std::size_t>(ptr - line_start);
      hash = mix(hash, line_len);
      hash = mix(hash, line_len == 0 ? 0u : static_cast<unsigned char>(*line_start));
      ++ptr;
    }
  }
  return hash;
}
