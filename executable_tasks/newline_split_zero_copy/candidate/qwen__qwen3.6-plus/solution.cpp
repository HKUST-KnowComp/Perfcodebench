#include "interface.h"
#include <cstring>
#include <cstdint>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  uint64_t hash = 0;
  const char* data = text.data();
  const char* const end_data = data + text.size();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    const char* p = data;
    while (p < end_data) {
      const char* nl = static_cast<const char*>(std::memchr(p, '\n', static_cast<std::size_t>(end_data - p)));
      std::size_t len = nl ? static_cast<std::size_t>(nl - p) : static_cast<std::size_t>(end_data - p);
      hash = mix(hash, len);
      hash = mix(hash, len == 0 ? 0u : static_cast<unsigned char>(p[0]));
      p = nl ? nl + 1 : end_data;
    }
  }
  return hash;
}
