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
    std::size_t end = start;
    while (end < n && data[end] != '\n') {
      ++end;
    }

    acc ^= static_cast<uint64_t>(XXH64(data + start, end - start, 0));
    acc *= kPrime;

    start = end + 1;
  }

  return acc;
}
