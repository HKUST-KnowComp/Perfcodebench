#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  struct LineInfo {
    std::size_t len;
    unsigned char first;
  };
  std::vector<LineInfo> lines;
  std::size_t start = 0;
  while (start < text.size()) {
    std::size_t end = text.find('\n', start);
    if (end == std::string::npos) {
      end = text.size();
    }
    std::size_t len = end - start;
    unsigned char first = (len > 0) ? static_cast<unsigned char>(text[start]) : 0;
    lines.push_back({len, first});
    start = end + 1;
  }

  constexpr uint64_t seed = 1469598103934665603ULL;
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = seed;
    for (const auto& info : lines) {
      hash = mix(hash, info.len);
      hash = mix(hash, info.first);
    }
  }
  return hash;
}
