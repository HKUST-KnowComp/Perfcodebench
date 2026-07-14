#include "interface.h"

#include <string>
#include <cstring>

namespace {
static inline uint64_t mix(uint64_t h, uint64_t v) {
  h ^= v;
  h *= 1099511628211ULL;
  return h;
}
}

uint64_t newline_split_checksum(const std::string& text, int iters) {
  const char* begin = text.data();
  const char* end = begin + text.size();
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    const char* p = begin;
    while (p < end) {
      const void* m = std::memchr(p, '\n', static_cast<size_t>(end - p));
      const char* q = m ? static_cast<const char*>(m) : end;
      std::size_t len = static_cast<std::size_t>(q - p);
      unsigned char fc = (len == 0) ? 0u : static_cast<unsigned char>(*p);
      h = mix(h, len);
      h = mix(h, fc);
      p = (q == end) ? end : (q + 1);
    }
    hash = h;
  }
  return hash;
}
