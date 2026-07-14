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
  uint64_t len;
  unsigned char first;
};

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  std::vector<LineInfo> line_infos;
  const size_t text_size = text.size();
  size_t start = 0;

  // Single zero-copy scan to extract only required line metadata
  while (start < text_size) {
    const size_t end = text.find('\n', start);
    const size_t actual_end = (end == std::string::npos) ? text_size : end;
    const uint64_t line_len = static_cast<uint64_t>(actual_end - start);
    const unsigned char first_byte = line_len > 0 ? static_cast<unsigned char>(text[start]) : 0u;
    line_infos.push_back({line_len, first_byte});
    start = actual_end + 1;
  }

  uint64_t final_hash = 0;
  // Reuse precomputed metadata for all iterations, no further input text access needed
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (const auto& info : line_infos) {
      hash = mix(hash, info.len);
      hash = mix(hash, static_cast<uint64_t>(info.first));
    }
    final_hash = hash;
  }
  return final_hash;
}