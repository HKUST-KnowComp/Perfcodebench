#include "interface.h"

#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  uint64_t total_length = 0;
  for (std::size_t s = 0; s + 1 < offsets.size(); ++s) {
    total_length += offsets[s + 1] - offsets[s];
  }
  std::vector<uint32_t> expanded;
  expanded.reserve(total_length * iters);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    expanded.clear();
    expanded.reserve(total_length);
    for (std::size_t s = 0; s + 1 < offsets.size(); ++s) {
      const uint32_t len = offsets[s + 1] - offsets[s];
      for (uint32_t i = 0; i < len; ++i) {
        expanded.push_back(static_cast<uint32_t>(s));
      }
    }
    uint64_t h = 1469598103934665603ULL;
    for (uint32_t value : expanded) {
      h = mix(h, value);
    }
    hash = iter == 0 ? h : mix(hash, h);
  }
  return hash;
}