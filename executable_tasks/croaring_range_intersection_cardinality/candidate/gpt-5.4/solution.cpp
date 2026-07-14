#include "interface.h"

#include <roaring.h>

#include <cstdint>
#include <vector>

namespace {

static inline void add_ranges(roaring_bitmap_t* bm,
                              const std::vector<uint32_t>& starts,
                              const std::vector<uint32_t>& lengths) {
  const std::size_t n = starts.size();
  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t len = lengths[i];
    if (len == 0) continue;
    const uint64_t start = starts[i];
    const uint64_t end = start + static_cast<uint64_t>(len);
    roaring_bitmap_add_range_closed(bm, start, end - 1);
  }
}

}  // namespace

uint64_t intersection_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
  roaring_bitmap_t* lhs = roaring_bitmap_create();
  roaring_bitmap_t* rhs = roaring_bitmap_create();

  add_ranges(lhs, lhs_starts, lhs_lengths);
  add_ranges(rhs, rhs_starts, rhs_lengths);

  roaring_bitmap_run_optimize(lhs);
  roaring_bitmap_run_optimize(rhs);

  const uint64_t result = roaring_bitmap_and_cardinality(lhs, rhs);

  roaring_bitmap_free(lhs);
  roaring_bitmap_free(rhs);
  return result;
}
