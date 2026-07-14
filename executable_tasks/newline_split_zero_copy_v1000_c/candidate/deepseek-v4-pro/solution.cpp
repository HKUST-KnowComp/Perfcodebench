#include "interface.h"

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
    const char* p = data;
    uint64_t h = 1469598103934665603ULL;
    while (p < end) {
      const char* line_start = p;
      while (p < end && *p != '\n') ++p;
      std::size_t len = p - line_start;
      h = mix(h, len);
      unsigned char fc = (len == 0) ? 0 : static_cast<unsigned char>(*line_start);
      h = mix(h, fc);
      if (p < end) ++p;  // skip the newline
    }
    hash = h;
  }
  return hash;
}