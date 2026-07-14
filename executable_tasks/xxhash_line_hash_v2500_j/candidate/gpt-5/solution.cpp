#include "interface.h"

#include "xxhash.h"

#include <cstring>
#include <string>
#include <cstdint>

uint64_t hash_lines(const std::string& input) {
  const char* data = input.data();
  const char* p = data;
  const char* const end = data + input.size();
  uint64_t acc = 1469598103934665603ULL;

  while (p < end) {
    const char* nl = static_cast<const char*>(std::memchr(p, '\n', static_cast<size_t>(end - p)));
    const char* seg_end = nl ? nl : end;
    size_t len = static_cast<size_t>(seg_end - p);

    uint64_t h = static_cast<uint64_t>(XXH64(p, len, 0));
    acc ^= h;
    acc *= 1099511628211ULL;

    if (!nl) break;
    p = nl + 1;
  }

  return acc;
}
