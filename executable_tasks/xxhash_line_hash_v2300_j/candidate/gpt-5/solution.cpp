#include "interface.h"

#include "xxhash.h"

#include <cstring>
#include <string>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;

  const char* ptr = input.data();
  const char* const end = ptr + input.size();

  while (ptr < end) {
    const void* nl = std::memchr(ptr, '\n', static_cast<size_t>(end - ptr));
    if (nl) {
      const char* nextnl = static_cast<const char*>(nl);
      const size_t len = static_cast<size_t>(nextnl - ptr);
      acc ^= static_cast<uint64_t>(XXH64(ptr, len, 0));
      acc *= 1099511628211ULL;
      ptr = nextnl + 1;
    } else {
      const size_t len = static_cast<size_t>(end - ptr);
      acc ^= static_cast<uint64_t>(XXH64(ptr, len, 0));
      acc *= 1099511628211ULL;
      break;
    }
  }

  return acc;
}
