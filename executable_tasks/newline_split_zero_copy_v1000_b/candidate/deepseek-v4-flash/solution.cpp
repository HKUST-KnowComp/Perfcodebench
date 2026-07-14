#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

void parse_lines(const std::string& text,
                  std::vector<std::size_t>& lengths,
                  std::vector<unsigned char>& first_chars) {
  lengths.clear();
  first_chars.clear();
  std::size_t start = 0;
  const std::size_t n = text.size();
  while (start < n) {
    // Find next newline manually for speed
    std::size_t end = start;
    while (end < n && text[end] != '\n') {
      ++end;
    }
    std::size_t len = end - start;
    lengths.push_back(len);
    first_chars.push_back(len == 0 ? 0 : static_cast<unsigned char>(text[start]));
    start = end + 1;
  }
}

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  // Precompute line data once
  std::vector<std::size_t> lengths;
  std::vector<unsigned char> first_chars;
  parse_lines(text, lengths, first_chars);

  const std::uint64_t FNV_OFFSET = 1469598103934665603ULL;
  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = FNV_OFFSET;
    const std::size_t num_lines = lengths.size();
    for (std::size_t i = 0; i < num_lines; ++i) {
      hash = mix(hash, lengths[i]);
      hash = mix(hash, first_chars[i]);
    }
    final_hash = hash;
  }
  return final_hash;
}