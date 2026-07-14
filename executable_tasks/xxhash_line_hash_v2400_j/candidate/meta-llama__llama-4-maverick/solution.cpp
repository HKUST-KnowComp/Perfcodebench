#include "interface.h"
#include "xxhash.h"

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  std::size_t start = 0;
  while (start < input.size()) {
    std::size_t end = input.find('\n', start);
    if (end == std::string::npos) {
      end = input.size();
    }
    acc ^= XXH64(input.data() + start, end - start, 0);
    acc *= 1099511628211ULL;
    start = end + 1;
  }
  return acc;
}