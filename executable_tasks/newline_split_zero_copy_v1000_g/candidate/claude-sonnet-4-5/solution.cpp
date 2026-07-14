#include "interface.h"

#include <string>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  std::vector<std::pair<std::size_t, std::size_t>> line_spans;
  line_spans.reserve(text.size() / 40);
  
  uint64_t hash = 0;
  const char* data = text.data();
  const std::size_t size = text.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    line_spans.clear();
    
    std::size_t start = 0;
    for (std::size_t i = 0; i < size; ++i) {
      if (data[i] == '\n') {
        line_spans.emplace_back(start, i);
        start = i + 1;
      }
    }
    if (start < size) {
      line_spans.emplace_back(start, size);
    }
    
    hash = 1469598103934665603ULL;
    for (const auto& span : line_spans) {
      std::size_t len = span.second - span.first;
      hash = mix(hash, len);
      unsigned char first_char = (len > 0) ? static_cast<unsigned char>(data[span.first]) : 0u;
      hash = mix(hash, first_char);
    }
  }
  
  return hash;
}