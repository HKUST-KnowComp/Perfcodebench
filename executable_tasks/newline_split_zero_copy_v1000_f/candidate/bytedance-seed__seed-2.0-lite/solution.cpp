#include "interface.h"

#include <string>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  const size_t text_size = text.size();
  uint64_t final_hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    size_t start = 0;
    
    while (start < text_size) {
      const size_t newline_pos = text.find('\n', start);
      const size_t line_end = (newline_pos == std::string::npos) ? text_size : newline_pos;
      const size_t line_len = line_end - start;
      const unsigned char first_char = line_len ? static_cast<unsigned char>(text[start]) : 0u;
      
      hash = mix(hash, line_len);
      hash = mix(hash, first_char);
      
      start = line_end + 1;
    }
    
    final_hash = hash;
  }
  
  return final_hash;
}