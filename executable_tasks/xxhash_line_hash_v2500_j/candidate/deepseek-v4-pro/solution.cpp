#include "interface.h"

#include "xxhash.h"

#include <cstring>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* ptr = input.data();
  std::size_t remaining = input.size();
  while (remaining > 0) {
    const char* nl = static_cast<const char*>(memchr(ptr, '\n', remaining));
    std::size_t line_len = nl ? static_cast<std::size_t>(nl - ptr) : remaining;
    uint64_t h = XXH64(ptr, line_len, 0);
    acc ^= h;
    acc *= 1099511628211ULL;
    if (!nl) break;
    ++nl;
    remaining -= (line_len + 1);
    ptr = nl;
  }
  return acc;
}