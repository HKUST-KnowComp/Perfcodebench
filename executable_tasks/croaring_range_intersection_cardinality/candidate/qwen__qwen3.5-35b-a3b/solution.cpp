#include <roaring.h>
#include <vector>
#include <cstdint>

uint64_t intersection_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
  roaring::RoaringBitmap lhs, rhs;
  for (std::size_t i = 0; i < lhs_starts.size(); ++i) {
    lhs.add_range(lhs_starts[i], lhs_starts[i] + lhs_lengths[i]);
  }
  for (std::size_t i = 0; i < rhs_starts.size(); ++i) {
    rhs.add_range(rhs_starts[i], rhs_starts[i] + rhs_lengths[i]);
  }
  return lhs.intersection_cardinality(rhs);
}