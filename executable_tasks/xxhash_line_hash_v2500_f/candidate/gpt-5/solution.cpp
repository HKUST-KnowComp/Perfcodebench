#include "interface.h"

#include "xxhash.h"

#include <string>
#include <cstring>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;

  const char* data = input.data();
  const char* cur = data;
  const char* end = data + input.size();

  while (cur < end) {
    const void* found = std::memchr(cur, '\n', static_cast<size_t>(end - cur));
    const char* nl = static_cast<const char*>(found);
    size_t len;
    if (nl) {
      len = static_cast<size_t>(nl - cur);
    } else {
      len = static_cast<size_t>(end - cur);
    }

    uint64_t h = static_cast<uint64_t>(XXH64(cur, len, 0));
    acc ^= h;
    acc *= 1099511628211ULL;

    if (!nl) break;
    cur = nl + 1;
  }

  return acc;
}
