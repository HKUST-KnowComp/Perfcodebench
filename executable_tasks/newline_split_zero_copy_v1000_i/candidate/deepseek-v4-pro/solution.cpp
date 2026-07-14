#include "interface.h"

#include <cstring>
#include <string>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  if (iters <= 0) {
    return 0;
  }

  uint64_t hash = 1469598103934665603ULL;
  const char* p = text.data();
  const char* end = p + text.size();

  while (p < end) {
    const char* nl = static_cast<const char*>(std::memchr(p, '\n', end - p));
    size_t len = (nl != nullptr) ? static_cast<size_t>(nl - p) : static_cast<size_t>(end - p);
    unsigned char first = (len > 0) ? static_cast<unsigned char>(*p) : 0;
    hash = mix(hash, len);
    hash = mix(hash, first);
    p = (nl != nullptr) ? (nl + 1) : end;
  }

  return hash;
}
