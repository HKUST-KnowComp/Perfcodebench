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
  if (iters <= 0) {
    return 0ULL;
  }

  const char* data = text.data();
  const char* cur = data;
  const char* end = data + text.size();
  const char* start = data;

  uint64_t hash = 1469598103934665603ULL;  // FNV offset basis

  while (cur < end) {
    if (*cur == '\n') {
      std::size_t len = static_cast<std::size_t>(cur - start);
      hash = mix(hash, static_cast<uint64_t>(len));
      uint64_t first = len ? static_cast<unsigned char>(*start) : 0u;
      hash = mix(hash, first);
      start = cur + 1;
    }
    ++cur;
  }

  if (start < end) {
    std::size_t len = static_cast<std::size_t>(end - start);
    hash = mix(hash, static_cast<uint64_t>(len));
    uint64_t first = static_cast<unsigned char>(*start);
    hash = mix(hash, first);
  }

  return hash;
}
