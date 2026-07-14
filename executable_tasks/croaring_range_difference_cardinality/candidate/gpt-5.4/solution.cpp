#include "interface.h"

#include <roaring.h>

#include <cstddef>
#include <cstdint>
#include <vector>

uint64_t difference_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
  roaring_bitmap_t* lhs = roaring_bitmap_create();
  roaring_bitmap_t* rhs = roaring_bitmap_create();

  for (std::size_t i = 0; i < lhs_starts.size(); ++i) {
    const uint32_t start = lhs_starts[i];
    const uint64_t end64 = static_cast<uint64_t>(start) + static_cast<uint64_t>(lhs_lengths[i]);
    if (end64 > static_cast<uint64_t>(start)) {
      const uint64_t capped = end64 > (static_cast<uint64_t>(UINT32_MAX) + 1ULL)
                                  ? (static_cast<uint64_t>(UINT32_MAX) + 1ULL)
                                  : end64;
      roaring_bitmap_add_range_closed(lhs, start, static_cast<uint32_t>(capped - 1));
    }
  }

  for (std::size_t i = 0; i < rhs_starts.size(); ++i) {
    const uint32_t start = rhs_starts[i];
    const uint64_t end64 = static_cast<uint64_t>(start) + static_cast<uint64_t>(rhs_lengths[i]);
    if (end64 > static_cast<uint64_t>(start)) {
      const uint64_t capped = end64 > (static_cast<uint64_t>(UINT32_MAX) + 1ULL)
                                  ? (static_cast<uint64_t>(UINT32_MAX) + 1ULL)
                                  : end64;
      roaring_bitmap_add_range_closed(rhs, start, static_cast<uint32_t>(capped - 1));
    }
  }

  const uint64_t result = roaring_bitmap_andnot_cardinality(lhs, rhs);

  roaring_bitmap_free(lhs);
  roaring_bitmap_free(rhs);
  return result;
}
