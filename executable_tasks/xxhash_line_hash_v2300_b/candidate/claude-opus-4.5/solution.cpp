#include "interface.h"

#include "xxhash.h"

#include <cstring>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* data = input.data();
  const char* end = data + input.size();
  const char* pos = data;
  
  while (pos < end) {
    const char* nl = static_cast<const char*>(std::memchr(pos, '\n', end - pos));
    const char* line_end = nl ? nl : end;
    std::size_t len = line_end - pos;
    acc ^= XXH64(pos, len, 0);
    acc *= 1099511628211ULL;
    pos = line_end + 1;
  }
  return acc;
}