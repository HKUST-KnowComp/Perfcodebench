#include "interface.h"

#include "xxhash.h"

#include <cstring>
#include <string>
#include <cstdint>

uint64_t hash_lines(const std::string& input) {
  const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  uint64_t acc = FNV_OFFSET_BASIS;

  const char* data = input.data();
  const char* end = data + input.size();
  const char* start = data;

  while (start < end) {
    const void* nlPtr = std::memchr(start, '\n', static_cast<size_t>(end - start));
    if (nlPtr) {
      const char* nl = static_cast<const char*>(nlPtr);
      size_t len = static_cast<size_t>(nl - start);
      XXH64_hash_t h = XXH64(start, len, 0);
      acc ^= static_cast<uint64_t>(h);
      acc *= FNV_PRIME;
      start = nl + 1;
    } else {
      // No more newlines; process the final segment if non-empty
      if (start < end) {
        size_t len = static_cast<size_t>(end - start);
        XXH64_hash_t h = XXH64(start, len, 0);
        acc ^= static_cast<uint64_t>(h);
        acc *= FNV_PRIME;
      }
      break;
    }
  }

  return acc;
}
