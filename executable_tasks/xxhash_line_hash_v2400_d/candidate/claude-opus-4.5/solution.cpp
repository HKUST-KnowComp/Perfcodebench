#include "interface.h"

#include "xxhash.h"

#include <cstring>
#include <string>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* data = input.data();
  const char* end = data + input.size();
  const char* start = data;
  
  while (start < end) {
    const char* nl = static_cast<const char*>(std::memchr(start, '\n', end - start));
    const char* line_end = nl ? nl : end;
    std::size_t len = line_end - start;
    acc ^= XXH64(start, len, 0);
    acc *= 1099511628211ULL;
    start = line_end + 1;
  }
  return acc;
}