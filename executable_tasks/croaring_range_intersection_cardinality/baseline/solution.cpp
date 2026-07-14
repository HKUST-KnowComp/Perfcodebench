#include "interface.h"

#include <unordered_set>

uint64_t intersection_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
  std::unordered_set<uint32_t> lhs;
  std::unordered_set<uint32_t> rhs;
  lhs.reserve(1 << 20);
  rhs.reserve(1 << 20);
  for (std::size_t i = 0; i < lhs_starts.size(); ++i) {
    const uint32_t begin = lhs_starts[i];
    const uint32_t end = begin + lhs_lengths[i];
    for (uint32_t v = begin; v < end; ++v) {
      lhs.insert(v);
    }
  }
  for (std::size_t i = 0; i < rhs_starts.size(); ++i) {
    const uint32_t begin = rhs_starts[i];
    const uint32_t end = begin + rhs_lengths[i];
    for (uint32_t v = begin; v < end; ++v) {
      rhs.insert(v);
    }
  }
  uint64_t total = 0;
  if (lhs.size() < rhs.size()) {
    for (uint32_t v : lhs) {
      total += rhs.find(v) != rhs.end() ? 1ULL : 0ULL;
    }
  } else {
    for (uint32_t v : rhs) {
      total += lhs.find(v) != lhs.end() ? 1ULL : 0ULL;
    }
  }
  return total;
}
