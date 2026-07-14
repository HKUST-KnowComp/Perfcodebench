#include "interface.h"

#include <string>
#include <vector>

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
    uint64_t iter_hash = 1469598103934665603ULL;
    std::size_t start = 0;
    while (start < text.size()) {
      std::size_t end = text.find('\n', start);
      if (end == std::string::npos) {
        end = text.size();
      }
      std::size_t line_len = end - start;
      iter_hash = mix(iter_hash, line_len);
      if (line_len > 0) {
        iter_hash = mix(iter_hash, static_cast<unsigned char>(text[start]));
      }
      start = end + 1;
    }
    hash = iter_hash; // The last iteration's hash is the final result
  }
  return hash;
}
