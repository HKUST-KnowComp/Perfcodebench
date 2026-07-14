#include "interface.h"
#include "xxhash.h"
#include <string>
#include <cstdint>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* data = input.data();
  const char* end_ptr = data + input.size();
  const char* start = data;

  while (start < end_ptr) {
    const char* current = start;
    while (current < end_ptr && *current != '\n') {
      ++current;
    }
    
    // Hash the segment directly from the input buffer
    acc ^= static_cast<uint64_t>(XXH64(start, static_cast<size_t>(current - start), 0));
    acc *= 1099511628211ULL;
    
    start = current + 1;
  }
  
  return acc;
}