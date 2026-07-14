#include "interface.h"
#include "roaring.h"
#include <vector>
#include <cstdint>

uint64_t difference_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
  RoaringBitmap lhs, rhs;
  
  for (std::size_t i = 0; i < lhs_starts.size(); ++i) {
    const uint32_t begin = lhs_starts[i];
    const uint32_t end = begin + lhs_lengths[i];
    lhs.add_range(begin, end);
  }
  
  for (std::size_t i = 0; i < rhs_starts.size(); ++i) {
    const uint32_t begin = rhs_starts[i];
    const uint32_t end = begin + rhs_lengths[i];
    rhs.add_range(begin, end);
  }
  
  lhs.subtract(rhs);
  return lhs.get_cardinality();
}