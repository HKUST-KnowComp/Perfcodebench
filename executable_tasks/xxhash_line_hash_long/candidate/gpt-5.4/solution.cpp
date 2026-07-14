#include "interface.h"

#include "xxhash.h"

#include <cstdint>
#include <string>

uint64_t hash_lines(const std::string& input) {
  static constexpr uint64_t kOffset = 1469598103934665603ULL;
  static constexpr uint64_t kPrime = 1099511628211ULL;

  uint64_t acc = kOffset;
  const char* data = input.data();
  const std::size_t n = input.size();

  std::size_t start = 0;
  while (start < n) {
    std::size_t pos = start;
    while (pos < n && data[pos] != '\n') {
      ++pos;
    }

    acc ^= static_cast<uint64_t>(XXH64(data + start, pos - start, 0));
    acc *= kPrime;

    start = (pos < n) ? (pos + 1) : pos;
  }

  return acc;
}
