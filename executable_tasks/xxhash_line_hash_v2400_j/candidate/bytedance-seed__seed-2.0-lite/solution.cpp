#include "interface.h"
#include "xxhash.h"
#include <string>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  std::size_t start = 0;
  const std::size_t input_size = input.size();
  const char* input_data = input.data();
  while (start < input_size) {
    std::size_t end = start;
    while (end < input_size && input_data[end] != '\n') {
      ++end;
    }
    const std::size_t line_len = end - start;
    acc ^= static_cast<uint64_t>(XXH64(input_data + start, line_len, 0));
    acc *= 1099511628211ULL;
    start = end + 1;
  }
  return acc;
}