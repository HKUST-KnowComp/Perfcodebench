#include "interface.h"

#include <string>
#include <cstdint>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  uint64_t hash = 0;
  const size_t len = text.size();
  const char* data = text.data();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    size_t start = 0;
    while (start < len) {
      size_t end = text.find('\n', start);
      if (end == std::string::npos) {
        end = len;
      }
      size_t line_len = end - start;
      hash = mix(hash, line_len);
      hash = mix(hash, line_len == 0 ? 0u : static_cast<unsigned char>(data[start]));
      start = end + 1;
    }
  }
  return hash;
}
