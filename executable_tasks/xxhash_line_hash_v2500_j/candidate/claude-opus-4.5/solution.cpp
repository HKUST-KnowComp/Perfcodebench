#include "interface.h"

#include "xxhash.h"

#include <cstring>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* data = input.data();
  const char* end = data + input.size();
  const char* start = data;
  
  while (start < end) {
    // Prefetch ahead for better cache utilization
    if (start + 256 < end) {
      __builtin_prefetch(start + 256, 0, 0);
    }
    
    // Use memchr for fast newline search (often SIMD-optimized)
    const char* nl = static_cast<const char*>(memchr(start, '\n', end - start));
    const char* line_end = nl ? nl : end;
    
    std::size_t len = line_end - start;
    acc ^= static_cast<uint64_t>(XXH64(start, len, 0));
    acc *= 1099511628211ULL;
    
    start = line_end + 1;
  }
  return acc;
}