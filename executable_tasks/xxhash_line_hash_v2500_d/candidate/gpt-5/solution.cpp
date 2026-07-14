#include "interface.h"

#include "xxhash.h"

#include <cstring>
#include <string>

uint64_t hash_lines(const std::string& input) {
  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME  = 1099511628211ULL;

  uint64_t acc = FNV_OFFSET;

  const char* data = input.data();
  const char* p = data;
  const char* end = data + input.size();

  while (p < end) {
    const void* nl = std::memchr(p, '\n', static_cast<size_t>(end - p));
    if (nl) {
      const char* q = static_cast<const char*>(nl);
      const size_t len = static_cast<size_t>(q - p);
      acc ^= static_cast<uint64_t>(XXH64(p, len, 0));
      acc *= FNV_PRIME;
      p = q + 1; // advance past newline
    } else {
      const size_t len = static_cast<size_t>(end - p);
      acc ^= static_cast<uint64_t>(XXH64(p, len, 0));
      acc *= FNV_PRIME;
      p = end; // done
    }
  }

  return acc;
}
