#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  if (iters == 0) {
    return 0;
  }

  // Precompute total length
  std::size_t total_len = 0;
  for (std::size_t s = 0; s + 1 < offsets.size(); ++s) {
    total_len += offsets[s + 1] - offsets[s];
  }

  // Build expanded segment-id array once
  std::vector<uint32_t> expanded;
  expanded.reserve(total_len);
  for (std::size_t s = 0; s + 1 < offsets.size(); ++s) {
    uint32_t seg_id = static_cast<uint32_t>(s);
    uint32_t len = offsets[s + 1] - offsets[s];
    for (uint32_t i = 0; i < len; ++i) {
      expanded.push_back(seg_id);
    }
  }

  // Compute hash
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t val : expanded) {
    hash = mix(hash, val);
  }
  return hash;
}