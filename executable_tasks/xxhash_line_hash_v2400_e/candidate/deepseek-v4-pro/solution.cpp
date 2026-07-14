#include "interface.h"

#include "xxhash.h"

#include <cstdint>
#include <cstring>
#include <string>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* data = input.data();
  const char* end = data + input.size();
  const char* p = data;
  while (p < end) {
      const char* nl = static_cast<const char*>(memchr(p, '\n', end - p));
      size_t line_len = nl ? static_cast<size_t>(nl - p) : static_cast<size_t>(end - p);
      uint64_t h = XXH64(p, line_len, 0);
      acc ^= h;
      acc *= 1099511628211ULL;
      p = nl ? nl + 1 : end;
  }
  return acc;
}