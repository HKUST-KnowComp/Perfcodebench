#include "interface.h"
#include "xxhash.h"
#include <string>
#include <cstring>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* data = input.data();
  const char* end_ptr = data + input.size();
  const char* curr = data;

  while (curr < end_ptr) {
    const char* next_nl = static_cast<const char*>(std::memchr(curr, '\n', end_ptr - curr));
    size_t len = next_nl ? (size_t)(next_nl - curr) : (size_t)(end_ptr - curr);
    
    acc ^= static_cast<uint64_t>(XXH64(curr, len, 0));
    acc *= 1099511628211ULL;
    
    if (!next_nl) break;
    curr = next_nl + 1;
  }
  
  return acc;
}