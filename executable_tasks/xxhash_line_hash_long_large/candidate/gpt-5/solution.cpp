#include "interface.h"

#include "xxhash.h"
#include <cstring>

uint64_t hash_lines(const std::string& input) {
  constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
  constexpr uint64_t FNV_PRIME  = 1099511628211ULL;

  uint64_t acc = FNV_OFFSET;
  const char* p = input.data();
  const char* const end = p + input.size();

  while (p < end) {
    const char* nl = static_cast<const char*>(std::memchr(p, '\n', static_cast<size_t>(end - p)));
    const size_t len = nl ? static_cast<size_t>(nl - p) : static_cast<size_t>(end - p);

    acc ^= static_cast<uint64_t>(XXH64(p, len, 0));
    acc *= FNV_PRIME;

    if (!nl) break;
    p = nl + 1;
  }

  return acc;
}
