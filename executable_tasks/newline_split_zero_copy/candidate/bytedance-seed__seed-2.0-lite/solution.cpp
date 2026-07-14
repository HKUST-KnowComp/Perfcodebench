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
  uint64_t hash = 0;
  const size_t text_size = text.size();
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    size_t start = 0;
    while (start < text_size) {
      const size_t end = text.find('\n', start);
      const size_t line_end = (end == std::string::npos) ? text_size : end;
      const size_t line_len = line_end - start;
      
      hash = mix(hash, line_len);
      if (line_len > 0) {
        hash = mix(hash, static_cast<unsigned char>(text[start]));
      } else {
        hash = mix(hash, 0u);
      }
      
      start = line_end + 1;
    }
  }
  return hash;
}