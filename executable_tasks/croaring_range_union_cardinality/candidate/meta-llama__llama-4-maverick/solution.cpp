#include "interface.h"
#include "roaring.h"

uint64_t union_cardinality(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& lengths) {
  RoaringBitmap result;
  for (std::size_t i = 0; i < starts.size(); ++i) {
    const uint32_t begin = starts[i];
    const uint32_t end = begin + lengths[i];
    result |= RoaringBitmap(begin, end);
  }
  return result.cardinality();
}