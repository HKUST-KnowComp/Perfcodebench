#include "interface.h"
#include "roaring.h"
#include <vector>

uint64_t union_cardinality(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& lengths) {
  if (starts.empty()) {
    return 0;
  }

  roaring::RoaringBitmap bitmap;
  for (std::size_t i = 0; i < starts.size(); ++i) {
    uint32_t start = starts[i];
    uint32_t length = lengths[i];
    if (length > 0) {
      // add_range is [start, end), so we use start + length
      bitmap.add_range(start, start + length);
    }
  }

  return static_cast<uint64_t>(bitmap.cardinality());
}