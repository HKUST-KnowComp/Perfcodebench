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
    const char* next_newline = static_cast<const char*>(memchr(current, '\n', end_ptr - current));
    size_t line_len = next_newline ? (next_newline - current) : (end_ptr - current);
    
    acc ^= static_cast<uint64_t>(XXH64(current, line_len, 0));
    acc *= 1099511628211ULL;
    
    if (!next_newline) break;
    current = next_newline + 1;
  }
  
  return acc;
}