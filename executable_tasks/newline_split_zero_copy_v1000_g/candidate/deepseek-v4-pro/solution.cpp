#include "interface.h"
#include <cstddef>
#include <cstring>

uint64_t newline_split_checksum(const std::string& text, int /*iters*/) {
  uint64_t hash = 1469598103934665603ULL;
  const char* data = text.data();
  std::size_t size = text.size();
  const char* end = data + size;
  const char* p = data;
  while (p < end) {
    const char* nl = static_cast<const char*>(std::memchr(p, '\n', end - p));
    if (!nl) nl = end;
    std::size_t len = nl - p;
    uint8_t first = (len > 0) ? static_cast<uint8_t>(*p) : 0;
    hash ^= len;
    hash *= 1099511628211ULL;
    hash ^= first;
    hash *= 1099511628211ULL;
    p = nl + 1;
  }
  return hash;
}