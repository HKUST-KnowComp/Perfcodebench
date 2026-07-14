#include "interface.h"
#include "xxhash.h"
#include <string>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  std::size_t start = 0;
  const std::size_t input_len = input.size();
  while (start < input_len) {
    std::size_t end = start;
    while (end < input_len && input[end] != '\n') {
      ++end;
    }
    const char* line_ptr = input.data() + start;
    const std::size_t line_len = end - start;
    acc ^= static_cast<uint64_t>(XXH64(line_ptr, line_len, 0));
    acc *= 1099511628211ULL;
    start = end + 1;
  }
  return acc;
}