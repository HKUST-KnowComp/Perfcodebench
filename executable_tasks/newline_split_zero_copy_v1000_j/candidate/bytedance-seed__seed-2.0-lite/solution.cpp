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
  if (iters <= 0) {
    return 0;
  }
  
  const size_t text_len = text.size();
  uint64_t hash = 1469598103934665603ULL;
  size_t start = 0;
  
  while (start < text_len) {
    const size_t end = text.find('\n', start);
    const size_t line_end = (end == std::string::npos) ? text_len : end;
    const size_t line_size = line_end - start;
    
    hash = mix(hash, line_size);
    if (line_size > 0) {
      hash = mix(hash, static_cast<unsigned char>(text[start]));
    } else {
      hash = mix(hash, 0u);
    }
    
    start = line_end + 1;
  }
  
  return hash;
}