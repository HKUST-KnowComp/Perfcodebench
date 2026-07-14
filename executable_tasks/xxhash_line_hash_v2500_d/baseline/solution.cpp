#include "interface.h"

#include "xxhash.h"

#include <string>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  std::size_t start = 0;
  while (start < input.size()) {
    std::size_t end = start;
    while (end < input.size() && input[end] != '\n') {
      ++end;
    }
    const std::string line = input.substr(start, end - start);
    acc ^= static_cast<uint64_t>(XXH64(line.data(), line.size(), 0));
    acc *= 1099511628211ULL;
    start = end + 1;
  }
  return acc;
}
