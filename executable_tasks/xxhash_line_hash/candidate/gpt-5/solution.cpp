#include "interface.h"
#include "xxhash.h"

#include <string>

uint64_t hash_lines(const std::string& input) {
  const char* ptr = input.data();
  const char* end = ptr + input.size();
  uint64_t acc = 1469598103934665603ULL;

  while (ptr < end) {
    const char* start = ptr;
    // Find end of line
    while (ptr < end && *ptr != '\n') {
      ++ptr;
    }
    const std::size_t len = static_cast<std::size_t>(ptr - start);
    const uint64_t h = static_cast<uint64_t>(XXH64(start, len, 0));
    acc ^= h;
    acc *= 1099511628211ULL;
    // Skip the newline character if present
    if (ptr < end) {
      ++ptr;
    }
  }

  return acc;
}
