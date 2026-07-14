#include "interface.h"
#include "xxhash.h"
#include <string>
#include <string_view>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* data = input.data();
  const size_t size = input.size();
  size_t start = 0;

  while (start < size) {
    size_t end = start;
    // Fast scan for newline
    while (end < size && data[end] != '\n') {
      ++end;
    }

    // Use XXH64 directly on the slice of the input string to avoid substr() allocation
    acc ^= XXH64(data + start, end - start, 0);
    acc *= 1099511628211ULL;

    start = end + 1;
  }
  return acc;
}