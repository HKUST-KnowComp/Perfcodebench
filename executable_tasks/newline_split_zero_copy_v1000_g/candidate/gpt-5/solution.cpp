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
  const std::size_t n = text.size();

  uint64_t hash = 0;  // Matches baseline behavior when iters == 0
  for (int iter = 0; iter < iters; ++iter) {
    const char* p = data;
    const char* const end = data + n;
    const char* line_start = p;

    uint64_t h = 1469598103934665603ULL;  // FNV offset basis

    while (p < end) {
      if (*p == '\n') {
        std::size_t len = static_cast<std::size_t>(p - line_start);
        h = mix(h, static_cast<uint64_t>(len));
        uint64_t first = (len == 0) ? 0u : static_cast<unsigned char>(*line_start);
        h = mix(h, first);
        line_start = p + 1;
      }
      ++p;
    }

    // Process the final line if there are remaining characters after the last newline
    if (line_start < end) {
      std::size_t len = static_cast<std::size_t>(end - line_start);
      h = mix(h, static_cast<uint64_t>(len));
      uint64_t first = (len == 0) ? 0u : static_cast<unsigned char>(*line_start);
      h = mix(h, first);
    }

    // Return the last iteration's hash, matching baseline semantics
    hash = h;
  }

  return hash;
}
