#include "interface.h"

#include "xxhash.h"

#include <cstring>
#include <string>

uint64_t hash_lines(const std::string& input) {
  const char* data = input.data();
  const std::size_t size = input.size();

  uint64_t acc = 1469598103934665603ULL; // FNV offset basis
  const uint64_t prime = 1099511628211ULL; // FNV prime

  const char* p = data;
  const char* end = data + size;

  while (p < end) {
    const char* nl = static_cast<const char*>(std::memchr(p, '\n', static_cast<size_t>(end - p)));
    if (nl) {
      const size_t len = static_cast<size_t>(nl - p);
      const uint64_t h = static_cast<uint64_t>(XXH64(p, len, 0));
      acc ^= h;
      acc *= prime;
      p = nl + 1;
    } else {
      const size_t len = static_cast<size_t>(end - p);
      const uint64_t h = static_cast<uint64_t>(XXH64(p, len, 0));
      acc ^= h;
      acc *= prime;
      break;
    }
  }

  return acc;
}
