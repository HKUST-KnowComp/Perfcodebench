#include "interface.h"
#include "roaring.h"

#include <cstdint>
#include <vector>

uint64_t xor_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
  // Create Roaring bitmaps for both sides
  roaring_bitmap_t* lhs = roaring_bitmap_create();
  roaring_bitmap_t* rhs = roaring_bitmap_create();

  if (lhs == nullptr || rhs == nullptr) {
    // Allocation failure fallback: no ranges processed
    if (lhs) roaring_bitmap_free(lhs);
    if (rhs) roaring_bitmap_free(rhs);
    return 0;
  }

  const uint64_t U32_LIMIT_PLUS_ONE = 1ULL << 32; // allow half-open end up to 2^32

  // Populate LHS bitmap with half-open ranges [start, start+length)
  for (size_t i = 0; i < lhs_starts.size(); ++i) {
    uint32_t len = lhs_lengths[i];
    if (len == 0) continue;
    uint64_t start = static_cast<uint64_t>(lhs_starts[i]);
    uint64_t end = start + static_cast<uint64_t>(len);
    if (end > U32_LIMIT_PLUS_ONE) end = U32_LIMIT_PLUS_ONE;
    if (end > start) {
      roaring_bitmap_add_range(lhs, start, end);
    }
  }

  // Populate RHS bitmap with half-open ranges [start, start+length)
  for (size_t i = 0; i < rhs_starts.size(); ++i) {
    uint32_t len = rhs_lengths[i];
    if (len == 0) continue;
    uint64_t start = static_cast<uint64_t>(rhs_starts[i]);
    uint64_t end = start + static_cast<uint64_t>(len);
    if (end > U32_LIMIT_PLUS_ONE) end = U32_LIMIT_PLUS_ONE;
    if (end > start) {
      roaring_bitmap_add_range(rhs, start, end);
    }
  }

  // Compute symmetric difference cardinality directly
  uint64_t result = roaring_bitmap_xor_cardinality(lhs, rhs);

  roaring_bitmap_free(lhs);
  roaring_bitmap_free(rhs);

  return result;
}
