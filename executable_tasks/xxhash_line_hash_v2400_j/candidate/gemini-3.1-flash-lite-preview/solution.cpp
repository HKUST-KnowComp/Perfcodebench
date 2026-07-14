#include "interface.h"
#include "xxhash.h"
#include <string>
#include <cstring>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* data = input.data();
  const char* end_ptr = data + input.size();
  const char* current = data;

  while (current < end_ptr) {
    const char* next_newline = static_cast<const char*>(std::memchr(current, '\n', end_ptr - current));
    const char* line_end = next_newline ? next_newline : end_ptr;
    
    size_t line_len = line_end - current;
    acc ^= static_cast<uint64_t>(XXH64(current, line_len, 0));
    acc *= 1099511628211ULL;
    
    current = line_end + 1;
  }
  
  return acc;
}