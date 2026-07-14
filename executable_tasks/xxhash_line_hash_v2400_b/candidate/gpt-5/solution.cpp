#include "interface.h"
#include "xxhash.h"

#include <cstring>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;

  const char* ptr = input.data();
  size_t remaining = input.size();

  while (remaining > 0) {
    const void* found = std::memchr(ptr, '\n', remaining);
    if (found) {
      const char* nl = static_cast<const char*>(found);
      size_t len = static_cast<size_t>(nl - ptr);
      uint64_t h = static_cast<uint64_t>(XXH64(ptr, len, 0));
      acc ^= h;
      acc *= 1099511628211ULL;
      // Advance past the newline
      size_t consumed = len + 1;
      ptr += consumed;
      remaining -= consumed;
    } else {
      // Last segment without a trailing newline
      uint64_t h = static_cast<uint64_t>(XXH64(ptr, remaining, 0));
      acc ^= h;
      acc *= 1099511628211ULL;
      break;
    }
  }

  return acc;
}
