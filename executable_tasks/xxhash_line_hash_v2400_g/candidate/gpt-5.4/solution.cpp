#include "interface.h"

#include "xxhash.h"

#include <cstdint>
#include <string>

uint64_t hash_lines(const std::string& input) {
  constexpr uint64_t kOffset = 1469598103934665603ULL;
  constexpr uint64_t kPrime = 1099511628211ULL;

  uint64_t acc = kOffset;
  const char* data = input.data();
  const std::size_t n = input.size();

  std::size_t start = 0;
  while (start < n) {
    const char* line_start = data + start;
    const char* p = line_start;
    const char* const endp = data + n;

    while (p < endp && *p != '\n') {
      ++p;
    }

    acc ^= static_cast<uint64_t>(XXH64(line_start, static_cast<size_t>(p - line_start), 0));
    acc *= kPrime;

    if (p == endp) {
      break;
    }
    start = static_cast<std::size_t>((p - data) + 1);
  }

  return acc;
}
