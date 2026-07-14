#include "interface.h"

#include "xxhash.h"
#include <cstring>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* __restrict__ p = input.data();
  const char* const end = p + input.size();
  const std::size_t total_size = input.size();
  
  if (total_size == 0) {
    return acc;
  }
  
  XXH64_state_t state;
  
  while (p < end) {
    const char* line_end = static_cast<const char*>(memchr(p, '\n', end - p));
    
    if (line_end == nullptr) {
      line_end = end;
    }
    
    const std::size_t line_len = line_end - p;
    
    XXH64_reset(&state, 0);
    XXH64_update(&state, p, line_len);
    const uint64_t hash = XXH64_digest(&state);
    
    acc ^= hash;
    acc *= 1099511628211ULL;
    
    p = (line_end < end) ? (line_end + 1) : line_end;
  }
  
  return acc;
}