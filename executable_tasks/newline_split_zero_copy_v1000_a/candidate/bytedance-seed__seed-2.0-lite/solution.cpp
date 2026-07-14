#include "interface.h"

#include <string>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

struct LineInfo {
  uint32_t len;
  uint8_t first_char;
};

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  // Precompute line metadata once to avoid repeated scanning and copies
  std::vector<LineInfo> line_infos;
  if (!text.empty()) {
    std::size_t start = 0;
    while (start < text.size()) {
      std::size_t end = text.find('\n', start);
      if (end == std::string::npos) {
        end = text.size();
      }
      const std::size_t line_len = end - start;
      const uint8_t first = line_len > 0 ? static_cast<unsigned char>(text[start]) : 0u;
      line_infos.push_back({static_cast<uint32_t>(line_len), first});
      start = end + 1;
    }
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (const auto& line : line_infos) {
      hash = mix(hash, line.len);
      hash = mix(hash, line.first_char);
    }
  }
  return hash;
}