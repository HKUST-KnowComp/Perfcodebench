#include "interface.h"

#include <vector>
#include <cstdint>
#include <string>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  // Precompute line lengths and first characters
  std::vector<std::size_t> line_lengths;
  std::vector<unsigned char> line_firsts;
  std::size_t start = 0;
  const std::string& s = text;
  while (start < s.size()) {
    std::size_t end = s.find('\n', start);
    if (end == std::string::npos) {
      end = s.size();
    }
    std::size_t len = end - start;
    line_lengths.push_back(len);
    if (len > 0) {
      line_firsts.push_back(static_cast<unsigned char>(s[start]));
    } else {
      line_firsts.push_back(0);
    }
    start = end + 1;
  }
  const std::size_t nlines = line_lengths.size();
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < nlines; ++i) {
      hash = mix(hash, static_cast<uint64_t>(line_lengths[i]));
      hash = mix(hash, static_cast<uint64_t>(line_firsts[i]));
    }
  }
  return hash;
}