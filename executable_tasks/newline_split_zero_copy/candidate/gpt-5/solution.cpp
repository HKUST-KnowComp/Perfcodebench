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
  const char* data = text.data();
  const std::size_t n = text.size();
  uint64_t result = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;  // FNV offset basis

    const char* p = data;
    const char* end = data + n;
    const char* line_start = p;

    for (; p != end; ++p) {
      if (*p == '\n') {
        std::size_t len = static_cast<std::size_t>(p - line_start);
        hash = mix(hash, static_cast<uint64_t>(len));
        uint64_t first = (len == 0) ? 0u : static_cast<uint64_t>(static_cast<unsigned char>(*line_start));
        hash = mix(hash, first);
        line_start = p + 1;
      }
    }

    if (line_start < end) {
      std::size_t len = static_cast<std::size_t>(end - line_start);
      hash = mix(hash, static_cast<uint64_t>(len));
      uint64_t first = static_cast<uint64_t>(static_cast<unsigned char>(*line_start));
      hash = mix(hash, first);
    }

    result = hash;  // match baseline: return the last iteration's hash
  }

  return result;
}
