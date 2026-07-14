#include "interface.h"

#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  const char* data = text.data();
  const size_t n = text.size();
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    size_t start = 0;
    for (size_t i = 0; i < n; ++i) {
      if (data[i] == '\n') {
        size_t len = i - start;
        hash = mix(hash, len);
        hash = mix(hash, len ? static_cast<unsigned char>(data[start]) : 0u);
        start = i + 1;
      }
    }
    if (start < n) {
      size_t len = n - start;
      hash = mix(hash, len);
      hash = mix(hash, len ? static_cast<unsigned char>(data[start]) : 0u);
    }
  }
  return hash;
}