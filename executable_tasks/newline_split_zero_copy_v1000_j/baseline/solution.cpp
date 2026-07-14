#include "interface.h"

#include <string>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  std::vector<std::string> lines;
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    lines.clear();
    std::size_t start = 0;
    while (start < text.size()) {
      std::size_t end = text.find('\n', start);
      if (end == std::string::npos) {
        end = text.size();
      }
      lines.push_back(text.substr(start, end - start));
      start = end + 1;
    }
    hash = 1469598103934665603ULL;
    for (const std::string& line : lines) {
      hash = mix(hash, line.size());
      hash = mix(hash, line.empty() ? 0u : static_cast<unsigned char>(line[0]));
    }
  }
  return hash;
}
