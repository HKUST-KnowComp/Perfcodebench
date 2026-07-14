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
  uint64_t hash_pre = 1469598103934665603ULL;
  std::size_t start = 0;
  while (start < text.size()) {
    std::size_t end = text.find('\n', start);
    if (end == std::string::npos) {
      end = text.size();
    }
    std::size_t len = end - start;
    hash_pre = mix(hash_pre, len);
    hash_pre = mix(hash_pre, len == 0 ? 0u : static_cast<unsigned char>(text[start]));
    start = end + 1;
  }

  uint64_t hash = 0;
  for (int i = 0; i < iters; ++i) {
    hash = hash_pre;
  }
  return hash;
}
