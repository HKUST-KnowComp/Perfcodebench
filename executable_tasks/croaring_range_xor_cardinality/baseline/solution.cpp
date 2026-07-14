#include "interface.h"

#include <unordered_set>

uint64_t xor_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
  std::unordered_set<uint32_t> lhs_values;
  std::unordered_set<uint32_t> rhs_values;
  for (std::size_t i = 0; i < lhs_starts.size(); ++i) {
    const uint32_t begin = lhs_starts[i];
    const uint32_t end = begin + lhs_lengths[i];
    for (uint32_t v = begin; v < end; ++v) {
      lhs_values.insert(v);
    }
  }
  for (std::size_t i = 0; i < rhs_starts.size(); ++i) {
    const uint32_t begin = rhs_starts[i];
    const uint32_t end = begin + rhs_lengths[i];
    for (uint32_t v = begin; v < end; ++v) {
      rhs_values.insert(v);
    }
  }
  uint64_t total = 0;
  for (uint32_t v : lhs_values) {
    total += rhs_values.find(v) == rhs_values.end() ? 1ULL : 0ULL;
  }
  for (uint32_t v : rhs_values) {
    total += lhs_values.find(v) == lhs_values.end() ? 1ULL : 0ULL;
  }
  return total;
}
