#include "interface.h"
#include <cstdint>
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
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    const char* data = text.data();
    const std::size_t len = text.size();
    std::size_t start = 0;
    while (start < len) {
      const char* nl = static_cast<const char*>(std::memchr(data + start, '\n', len - start));
      std::size_t end;
      if (nl) {
        end = nl - data;
      } else {
        end = len;
      }
      std::size_t line_len = end - start;
      hash = mix(hash, static_cast<uint64_t>(line_len));
      unsigned char first_char = (line_len == 0) ? 0u : static_cast<unsigned char>(data[start]);
      hash = mix(hash, static_cast<uint64_t>(first_char));
      start = end + 1;
    }
  }
  return hash;
}