#include "interface.h"
#include <cstring>

namespace {

constexpr uint64_t kInitialHash = 1469598103934665603ULL;

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  const char* data = text.data();
  const std::size_t size = text.size();
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = kInitialHash;
    std::size_t start = 0;
    while (start < size) {
      const char* nl = static_cast<const char*>(memchr(data + start, '\n', size - start));
      std::size_t end = (nl != nullptr) ? static_cast<std::size_t>(nl - data) : size;
      std::size_t len = end - start;
      hash = mix(hash, len);
      hash = mix(hash, (len == 0) ? uint64_t(0) : static_cast<uint64_t>(static_cast<unsigned char>(data[start])));
      start = end + 1;
    }
  }
  return hash;
}