#include "interface.h"

#include "xxhash.h"

#include <cstring>

uint64_t hash_lines(const std::string& input) {
  const char* ptr = input.data();
  const char* end = ptr + input.size();

  uint64_t acc = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  while (ptr < end) {
    const char* nl = static_cast<const char*>(std::memchr(ptr, '\n', static_cast<size_t>(end - ptr)));
    size_t len = nl ? static_cast<size_t>(nl - ptr) : static_cast<size_t>(end - ptr);

    uint64_t h = static_cast<uint64_t>(XXH64(ptr, len, 0));
    acc ^= h;
    acc *= FNV_PRIME;

    if (nl) {
      ptr = nl + 1;
    } else {
      // Mirror baseline behavior: advance past end to terminate loop
      ptr = end + 1;
    }
  }

  return acc;
}
