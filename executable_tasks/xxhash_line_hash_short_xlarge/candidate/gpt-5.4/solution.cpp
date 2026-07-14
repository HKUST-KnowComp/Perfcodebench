#include "interface.h"

#include "xxhash.h"

#include <cstdint>
#include <string>

uint64_t hash_lines(const std::string& input) {
  constexpr uint64_t kOffset = 1469598103934665603ULL;
  constexpr uint64_t kPrime = 1099511628211ULL;

  uint64_t acc = kOffset;

  const char* p = input.data();
  const char* const end = p + input.size();

  while (p < end) {
    const char* q = p;

    while (q + 8 <= end) {
      uint64_t v;
      __builtin_memcpy(&v, q, sizeof(v));
      const uint64_t x = v ^ 0x0A0A0A0A0A0A0A0AULL;
      if (((x - 0x0101010101010101ULL) & ~x & 0x8080808080808080ULL) != 0) {
        break;
      }
      q += 8;
    }

    while (q < end && *q != '\n') {
      ++q;
    }

    acc ^= static_cast<uint64_t>(XXH64(p, static_cast<size_t>(q - p), 0));
    acc *= kPrime;

    p = (q < end) ? (q + 1) : q;
  }

  return acc;
}
