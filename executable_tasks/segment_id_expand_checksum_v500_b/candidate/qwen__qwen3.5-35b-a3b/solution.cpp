#pragma once

#include <cstdint>
#include <vector>

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  std::vector<uint32_t> expanded;
  for (std::size_t s = 0; s + 1 < offsets.size(); ++s) {
    const uint32_t len = offsets[s + 1] - offsets[s];
    expanded.insert(expanded.end(), len, static_cast<uint32_t>(s));
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t value : expanded) {
      hash ^= value;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}