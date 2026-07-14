#include "interface.h"

#include <unordered_set>

uint64_t difference_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
  std::unordered_set<uint32_t> values;
  for (std::size_t i = 0; i < lhs_starts.size(); ++i) {
    const uint32_t begin = lhs_starts[i];
    const uint32_t end = begin + lhs_lengths[i];
    for (uint32_t v = begin; v < end; ++v) {
      values.insert(v);
    }
  }
  for (std::size_t i = 0; i < rhs_starts.size(); ++i) {
    const uint32_t begin = rhs_starts[i];
    const uint32_t end = begin + rhs_lengths[i];
    for (uint32_t v = begin; v < end; ++v) {
      values.erase(v);
    }
  }
  return static_cast<uint64_t>(values.size());
}
