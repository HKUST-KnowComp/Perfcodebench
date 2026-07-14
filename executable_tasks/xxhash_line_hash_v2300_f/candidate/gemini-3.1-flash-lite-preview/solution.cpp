#include "interface.h"
#include "xxhash.h"
#include <string>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* data = input.data();
  const char* end_ptr = data + input.size();
  const char* start = data;

  while (start < end_ptr) {
    const char* next_newline = static_cast<const char*>(memchr(start, '\n', end_ptr - start));
    const char* end = (next_newline != nullptr) ? next_newline : end_ptr;
    
    size_t len = end - start;
    uint64_t line_hash = XXH64(start, len, 0);
    
    acc ^= line_hash;
    acc *= 1099511628211ULL;
    
    start = end + 1;
  }
  
  return acc;
}