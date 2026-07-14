#include "interface.h"

#include <string>
#include <string_view>
#include <vector>
#include <cstring>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  std::vector<std::string_view> lines;
  lines.reserve(1024);
  
  uint64_t hash = 1469598103934665603ULL;
  
  for (int iter = 0; iter < iters; ++iter) {
    lines.clear();
    
    const char* data = text.data();
    const char* end = data + text.size();
    const char* start = data;
    
    while (start < end) {
      const char* newline = static_cast<const char*>(memchr(start, '\n', end - start));
      if (newline) {
        lines.emplace_back(start, newline - start);
        start = newline + 1;
      } else {
        lines.emplace_back(start, end - start);
        break;
      }
    }
    
    hash = 1469598103934665603ULL;
    for (const std::string_view& line : lines) {
      hash = mix(hash, line.size());
      hash = mix(hash, line.empty() ? 0u : static_cast<unsigned char>(line[0]));
    }
  }
  
  return hash;
}