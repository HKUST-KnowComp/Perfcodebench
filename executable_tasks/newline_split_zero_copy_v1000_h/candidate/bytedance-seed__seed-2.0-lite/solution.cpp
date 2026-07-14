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
  const std::size_t text_size = text.size();
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    std::size_t start = 0;
    while (start < text_size) {
      const std::size_t end = text.find('\n', start);
      const std::size_t line_end = (end == std::string::npos) ? text_size : end;
      const uint64_t line_len = static_cast<uint64_t>(line_end - start);
      const uint64_t first_char = (start < line_end) ? static_cast<unsigned char>(text[start]) : 0u;
      hash = mix(hash, line_len);
      hash = mix(hash, first_char);
      start = line_end + 1;
    }
  }
  return hash;
}