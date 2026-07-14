#include "interface.h"

#include "xxhash.h"

#include <cstring>
#include <string>
#include <cstdint>

uint64_t hash_lines(const std::string& input) {
  const char* ptr = input.data();
  const char* const end = ptr + input.size();

  uint64_t acc = 1469598103934665603ULL; // FNV offset basis
  const uint64_t prime = 1099511628211ULL; // FNV prime

  while (ptr < end) {
    const void* found = std::memchr(ptr, '\n', static_cast<size_t>(end - ptr));
    const char* nl = static_cast<const char*>(found);

    size_t len = nl ? static_cast<size_t>(nl - ptr) : static_cast<size_t>(end - ptr);

    uint64_t h = static_cast<uint64_t>(XXH64(ptr, len, 0));
    acc ^= h;
    acc *= prime;

    if (!nl) break;
    ptr = nl + 1;
  }

  return acc;
}
