#include "interface.h"
#include "roaring.h"

uint64_t xor_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
    
    roaring_bitmap_t* lhs = roaring_bitmap_create();
    roaring_bitmap_t* rhs = roaring_bitmap_create();

    for (size_t i = 0; i < lhs_starts.size(); ++i) {
        uint32_t start = lhs_starts[i];
        uint32_t end = start + lhs_lengths[i];
        // CRoaring add_range is [start, end), matching the baseline logic
        roaring_bitmap_add_range(lhs, start, end);
    }

    for (size_t i = 0; i < rhs_starts.size(); ++i) {
        uint32_t start = rhs_starts[i];
        uint32_t end = start + rhs_lengths[i];
        roaring_bitmap_add_range(rhs, start, end);
    }

    // XOR cardinality: |A XOR B| = |A| + |B| - 2 * |A AND B|
    uint64_t lhs_card = roaring_bitmap_cardinality(lhs);
    uint64_t rhs_card = roaring_bitmap_cardinality(rhs);
    
    // Create a copy of lhs to perform intersection without destroying original
    roaring_bitmap_t* intersection = roaring_bitmap_create();
    roaring_bitmap_and(intersection, lhs, rhs);
    uint64_t intersect_card = roaring_bitmap_cardinality(intersection);

    roaring_bitmap_destroy(lhs);
    roaring_bitmap_destroy(rhs);
    roaring_bitmap_destroy(intersection);

    return lhs_card + rhs_card - (2 * intersect_card);
}