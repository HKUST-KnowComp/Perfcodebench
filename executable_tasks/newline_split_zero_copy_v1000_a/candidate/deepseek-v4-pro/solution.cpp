#include "interface.h"

#include <cstdint>
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
  // Precompute line lengths and first characters (zero-copy, one pass)
  std::vector<size_t> lengths;
  std::vector<unsigned char> first_chars;

  const char* data = text.data();
  const char* const end_ptr = data + text.size();
  const char* start = data;

  while (start < end_ptr) {
    // find next newline or end of buffer
    const char* newline = start;
    while (newline < end_ptr && *newline != '\n') ++newline;
    const size_t len = newline - start;
    lengths.push_back(len);
    first_chars.push_back(len ? static_cast<unsigned char>(start[0]) : 0);
    start = newline + 1;  // skip delimiter if present, else exit (newline == end_ptr -> start > end_ptr)
  }

  // Re-hash precomputed attributes for each iteration
  uint64_t hash = 0;
  const size_t num_lines = lengths.size();
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < num_lines; ++i) {
      hash = mix(hash, lengths[i]);
      hash = mix(hash, first_chars[i]);
    }
  }

  return hash;
}