#include "interface.h"
#include "xxhash.h"
#include <string>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* data = input.data();
  size_t size = input.size();
  size_t start = 0;

  while (start < size) {
    size_t end = input.find('\n', start);
    if (end == std::string::npos) {
      end = size;
    }

    // Use the pointer and length directly to avoid std::string allocation/copy
    acc ^= static_cast<uint64_t>(XXH64(data + start, end - start, 0));
    acc *= 1099511628211ULL;

    start = end + 1;
  }
  return acc;
}