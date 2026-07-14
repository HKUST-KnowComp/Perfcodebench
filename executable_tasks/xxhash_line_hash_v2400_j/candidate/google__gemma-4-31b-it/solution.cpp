#include "interface.h"
#include "xxhash.h"
#include <string>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* data = input.data();
  size_t size = input.size();
  size_t start = 0;

  while (start < size) {
    size_t end = start;
    while (end < size && data[end] != '\n') {
      ++end;
    }
    
    // Use the pointer and length directly to avoid std::string allocation
    acc ^= static_cast<uint64_t>(XXH64(data + start, end - start, 0));
    acc *= 1099511628211ULL;
    
    start = end + 1;
  }
  return acc;
}