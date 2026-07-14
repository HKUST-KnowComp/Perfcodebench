#include "interface.h"

#include "xxhash.h"
#include <cstring>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* p = input.data();
  const char* const end = p + input.size();
  const uint64_t mul = 1099511628211ULL;
  while (p < end) {
    size_t remaining = end - p;
    const char* nl = static_cast<const char*>(std::memchr(p, '\n', remaining));
    size_t len = (nl ? nl : end) - p;
    acc ^= static_cast<uint64_t>(XXH64(p, len, 0));
    acc *= mul;
    p = (nl != nullptr) ? (nl + 1) : end;
  }
  return acc;
}