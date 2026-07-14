#include "interface.h"

#include <string>
#include <vector>

namespace {

struct LineSpan {
  std::size_t start;
  std::size_t length;
};

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  std::vector<LineSpan> lines;
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    lines.clear();
    
    const char* data = text.data();
    const std::size_t size = text.size();
    std::size_t start = 0;
    
    while (start < size) {
      std::size_t end = start;
      while (end < size && data[end] != '\n') {
        ++end;
      }
      lines.push_back({start, end - start});
      start = end + 1;
    }
    
    hash = 1469598103934665603ULL;
    for (const LineSpan& line : lines) {
      hash = mix(hash, line.length);
      hash = mix(hash, line.length == 0 ? 0u : static_cast<unsigned char>(data[line.start]));
    }
  }
  
  return hash;
}