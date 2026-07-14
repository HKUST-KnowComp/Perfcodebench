#include "interface.h"

#include "xxhash.h"

#include <cstring>
#include <string>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* data = input.data();
  const char* end = data + input.size();
  
  while (data < end) {
    const char* nl = static_cast<const char*>(std::memchr(data, '\n', end - data));
    const char* line_end = nl ? nl : end;
    std::size_t len = line_end - data;
    
    acc ^= XXH64(data, len, 0);
    acc *= 1099511628211ULL;
    
    data = line_end + 1;
  }
  return acc;
}