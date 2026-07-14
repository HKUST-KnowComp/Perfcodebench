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
  const char* data = text.data();
  const char* end = data + text.size();
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    const char* p = data;
    const char* line_start = p;
    while (p < end) {
      if (*p == '\n') {
        std::size_t len = static_cast<std::size_t>(p - line_start);
        unsigned char first = len ? static_cast<unsigned char>(*line_start) : 0u;
        hash = mix(hash, len);
        hash = mix(hash, first);
        ++p;
        line_start = p;
      } else {
        ++p;
      }
    }
    if (line_start < end) {
      std::size_t len = static_cast<std::size_t>(end - line_start);
      unsigned char first = static_cast<unsigned char>(*line_start);
      hash = mix(hash, len);
      hash = mix(hash, first);
    }
  }
  return hash;
}
