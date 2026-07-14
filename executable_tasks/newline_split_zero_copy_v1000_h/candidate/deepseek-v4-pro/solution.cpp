#include "interface.h"

#include <string>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    std::size_t start = 0;
    const std::size_t len = text.size();
    while (start < len) {
      std::size_t end = text.find('\n', start);
      if (end == std::string::npos) end = len;
      std::size_t line_len = end - start;
      h = mix(h, line_len);
      h = mix(h, (line_len ? static_cast<unsigned char>(text[start]) : 0u));
      start = end + 1;
    }
    hash = h;
  }
  return hash;
}
