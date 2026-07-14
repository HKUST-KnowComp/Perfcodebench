#include "interface.h"

#include "xxhash.h"
#include <cstring>

uint64_t hash_lines(const std::string& input) {
  static constexpr uint64_t kOffset = 1469598103934665603ULL;
  static constexpr uint64_t kPrime = 1099511628211ULL;

  uint64_t acc = kOffset;

  const char* p = input.data();
  size_t remaining = input.size();

  while (remaining != 0) {
    const void* nl = std::memchr(p, '\n', remaining);
    if (nl != nullptr) {
      const char* line_end = static_cast<const char*>(nl);
      size_t len = static_cast<size_t>(line_end - p);
      acc ^= static_cast<uint64_t>(XXH64(p, len, 0));
      acc *= kPrime;
      size_t adv = len + 1; // include the newline
      p += adv;
      remaining -= adv;
    } else {
      // Last line (no trailing newline)
      acc ^= static_cast<uint64_t>(XXH64(p, remaining, 0));
      acc *= kPrime;
      break;
    }
  }

  return acc;
}
