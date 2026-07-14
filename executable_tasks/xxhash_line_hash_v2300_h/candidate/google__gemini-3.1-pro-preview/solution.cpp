#include "interface.h"
#include "xxhash.h"
#include <string>
#include <cstring>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* ptr = input.data();
  const char* end = ptr + input.size();
  
  while (ptr < end) {
    const char* nl = static_cast<const char*>(std::memchr(ptr, '\n', end - ptr));
    if (!nl) {
      nl = end;
    }
    acc ^= static_cast<uint64_t>(XXH64(ptr, nl - ptr, 0));
    acc *= 1099511628211ULL;
    ptr = nl + 1;
  }
  return acc;
}
