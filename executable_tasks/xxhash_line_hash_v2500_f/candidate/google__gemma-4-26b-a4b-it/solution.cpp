#include "interface.h"
#include "xxhash.h"
#include <string"

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* data = input.data();
  size_t size = input.size();
  size_t start = 0;

  while (start < size) {
    size_t end = start;
    // Use find for faster scanning of the newline character
    size_t next_newline = input.find('\n', start);
    
    if (next_newline == std::string::npos) {
      end = size;
    } else {
      end = next_newline;
    }

    size_t line_len = end - start;
    // Avoid std::string construction/copying; use direct pointer access
    acc ^= XXH64(data + start, line_len, 0);
    acc *= 1099511628211ULL;

    if (next_newline == std::string::npos) {
      break;
    }
    start = next_newline + 1;
  }

  return acc;
}