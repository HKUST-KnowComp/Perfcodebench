#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Use a bitset-like approach for small code spaces
    // Assuming codes fit in uint16_t (max 65536 values)
    constexpr size_t MAX_CODES = 65536;
    static thread_local std::vector<bool> seen_codes;
    
    if (seen_codes.size() < MAX_CODES) {
      seen_codes.resize(MAX_CODES, false);
    }
    
    // Reset only the range we might use
    size_t max_code = 0;
    for (uint16_t code : codes) {
      if (code > max_code) max_code = code;
    }
    
    for (size_t i = 0; i <= max_code; ++i) {
      seen_codes[i] = false;
    }
    
    // Mark seen codes
    for (uint16_t code : codes) {
      seen_codes[code] = true;
    }
    
    // Count distinct codes
    count = 0;
    for (size_t i = 0; i <= max_code; ++i) {
      if (seen_codes[i]) {
        ++count;
      }
    }
  }
  return count;
}