#include "interface.h"
#include "roaring.h"

#include <cstddef>
#include <cstdint>

uint64_t difference_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
  // Fast path: no LHS ranges implies empty result.
  if (lhs_starts.empty()) {
    return 0;
  }

  roaring_bitmap_t* bmp = roaring_bitmap_create();
  if (!bmp) {
    return 0; // Allocation failure fallback.
  }

  const uint64_t max_exclusive = 1ULL << 32; // Values are 32-bit; valid universe is [0, 2^32).

  // Add all LHS ranges (half-open [start, end)).
  const std::size_t lhs_n = lhs_starts.size();
  for (std::size_t i = 0; i < lhs_n; ++i) {
    uint32_t s32 = lhs_starts[i];
    uint32_t len = lhs_lengths[i];
    if (len == 0) continue;
    uint64_t start = static_cast<uint64_t>(s32);
    uint64_t end = start + static_cast<uint64_t>(len);
    if (end > max_exclusive) end = max_exclusive; // Clip to valid domain
    if (start < end) {
      roaring_bitmap_add_range(bmp, start, end);
    }
  }

  // Remove all RHS ranges from the bitmap (half-open [start, end)).
  const std::size_t rhs_n = rhs_starts.size();
  for (std::size_t i = 0; i < rhs_n; ++i) {
    uint32_t s32 = rhs_starts[i];
    uint32_t len = rhs_lengths[i];
    if (len == 0) continue;
    uint64_t start = static_cast<uint64_t>(s32);
    uint64_t end = start + static_cast<uint64_t>(len);
    if (end > max_exclusive) end = max_exclusive; // Clip to valid domain
    if (start < end) {
      roaring_bitmap_remove_range(bmp, start, end);
    }
  }

  uint64_t result = roaring_bitmap_get_cardinality(bmp);
  roaring_bitmap_free(bmp);
  return result;
}
