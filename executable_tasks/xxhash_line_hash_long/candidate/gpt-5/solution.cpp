#include "interface.h"

#include "xxhash.h"

#include <cstring>
#include <cstdint>
#include <string>

uint64_t hash_lines(const std::string& input) {
  const char* p = input.data();
  const char* const end = p + input.size();

  uint64_t acc = 1469598103934665603ULL; // FNV offset basis
  const uint64_t fnv_prime = 1099511628211ULL;

  while (p < end) {
    const size_t remaining = static_cast<size_t>(end - p);
    const void* nl = std::memchr(p, '\n', remaining);
    const size_t len = nl ? static_cast<size_t>(static_cast<const char*>(nl) - p)
                          : remaining;

    const uint64_t h = static_cast<uint64_t>(XXH64(p, len, 0));
    acc ^= h;
    acc *= fnv_prime;

    if (!nl) break;
    p = static_cast<const char*>(nl) + 1;
  }

  return acc;
}
