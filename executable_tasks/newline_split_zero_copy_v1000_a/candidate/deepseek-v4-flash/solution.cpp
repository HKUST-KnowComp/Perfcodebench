#include "interface.h"

#include <cstdint>
#include <cstring>
#include <string>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  const char* data = text.data();
  std::size_t size = text.size();
  const uint64_t base_hash = 1469598103934665603ULL;
  uint64_t hash = base_hash;

  for (int iter = 0; iter < iters; ++iter) {
    hash = base_hash;
    const char* p = data;
    const char* end = data + size;

    while (p < end) {
      const char* nl = static_cast<const char*>(memchr(p, '\n', end - p));
      if (!nl) nl = end;

      std::size_t line_len = nl - p;
      hash = mix(hash, line_len);
      hash = mix(hash, line_len ? static_cast<unsigned char>(*p) : 0u);

      p = nl + 1;
    }
  }
  return hash;
}
