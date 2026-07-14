#include "interface.h"
#include "xxhash.h"
#include <string>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* data = input.data();
  const char* end = data + input.size();
  const char* start = data;

  while (start < end) {
    const char* next = static_cast<const char*>(memchr(start, '\n', end - start));
    size_t len = (next) ? (size_t)(next - start) : (size_t)(end - start);
    
    acc ^= static_cast<uint64_t>(XXH64(start, len, 0));
    acc *= 1099511628211ULL;
    
    if (!next) break;
    start = next + 1;
  }
  return acc;
}