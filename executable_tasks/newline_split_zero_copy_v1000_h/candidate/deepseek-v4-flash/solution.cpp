#include "interface.h"
#include <cstdint>
#include <cstring>
#include <string>

namespace {

// FNV-1a mixing function, identical to baseline
uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // anonymous namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  const char* data = text.data();
  std::size_t len = text.size();
  uint64_t result = 0;  // Will hold last iteration's hash

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;  // FNV offset basis
    const char* p = data;
    const char* end = data + len;

    while (p < end) {
      // Find the next newline (or end of buffer)
      const char* next = static_cast<const char*>(
          std::memchr(p, '\n', static_cast<std::size_t>(end - p)));
      if (next == nullptr) {
        next = end;
      }

      std::size_t line_len = static_cast<std::size_t>(next - p);
      hash = mix(hash, line_len);
      hash = mix(hash, (line_len == 0) ? 0 : static_cast<unsigned char>(*p));

      p = next + 1;  // move past the newline
    }

    result = hash;
  }

  return result;
}
