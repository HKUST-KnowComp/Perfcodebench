#include "interface.h"

#include "xxhash.h"

#include <cstdint>
#include <string>

uint64_t hash_lines(const std::string& input) {
  static constexpr uint64_t kOffsetBasis = 1469598103934665603ULL;
  static constexpr uint64_t kPrime = 1099511628211ULL;

  uint64_t acc = kOffsetBasis;

  const char* const data = input.data();
  const std::size_t size = input.size();
  const char* p = data;
  const char* const end = data + size;

  while (p < end) {
    const char* line_start = p;

    while (p < end && *p != '\n') {
      ++p;
    }

    acc ^= static_cast<uint64_t>(XXH64(line_start, static_cast<std::size_t>(p - line_start), 0));
    acc *= kPrime;

    if (p < end) {
      ++p;
    }
  }

  return acc;
}
