#include "interface.h"

#include "xxhash.h"
#include <cstring>

#include <string>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;

  const char* data = input.data();
  const char* end = data + input.size();
  const char* s = data;

  while (s < end) {
    const void* nl = std::memchr(s, '\n', static_cast<size_t>(end - s));
    const char* p = nl ? static_cast<const char*>(nl) : end;

    uint64_t hv = static_cast<uint64_t>(XXH64(s, static_cast<size_t>(p - s), 0));
    acc ^= hv;
    acc *= 1099511628211ULL;

    if (!nl) break; // no more newlines; done
    s = p + 1;      // move past newline
  }

  return acc;
}
