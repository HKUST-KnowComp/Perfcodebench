#include "interface.h"

#include <cstdint>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  const char* data = text.data();
  const std::size_t len = text.size();
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    const char* ptr = data;
    const char* end = data + len;

    while (ptr < end) {
      const char* nl = static_cast<const char*>(std::memchr(ptr, '\n', end - ptr));
      const char* line_end = nl ? nl : end;
      std::size_t line_len = static_cast<std::size_t>(line_end - ptr);
      hash = mix(hash, line_len);
      hash = mix(hash, line_len == 0 ? 0u : static_cast<unsigned char>(*ptr));
      ptr = line_end + 1;
    }
  }

  return hash;
}
