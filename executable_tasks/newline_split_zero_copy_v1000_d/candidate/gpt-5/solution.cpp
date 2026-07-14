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
  const char* data = text.data();
  const std::size_t n = text.size();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;

    const char* start = data;
    const char* p = data;
    const char* end = data + n;

    while (p != end) {
      if (*p == '\n') {
        std::size_t len = static_cast<std::size_t>(p - start);
        hash = mix(hash, static_cast<uint64_t>(len));
        unsigned char first = len ? static_cast<unsigned char>(*start) : 0u;
        hash = mix(hash, static_cast<uint64_t>(first));
        start = p + 1;
      }
      ++p;
    }

    if (start < end) {
      std::size_t len = static_cast<std::size_t>(end - start);
      hash = mix(hash, static_cast<uint64_t>(len));
      unsigned char first = len ? static_cast<unsigned char>(*start) : 0u;
      hash = mix(hash, static_cast<uint64_t>(first));
    }
  }

  return hash;
}
