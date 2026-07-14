#include "interface.h"
#include "roaring.h"
#include <vector>

uint64_t intersection_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
    
    roaring_bitmap_t* lhs_bitmap = roaring_bitmap_create();
    roaring_bitmap_t* rhs_bitmap = roaring_bitmap_create();

    for (std::size_t i = 0; i < lhs_starts.size(); ++i) {
        if (lhs_lengths[i] > 0) {
            roaring_bitmap_add_range(lhs_bitmap, lhs_starts[i], lhs_starts[i] + lhs_lengths[i]);
        }
    }

    for (std::size_t i = 0; i < rhs_starts.size(); ++i) {
        if (rhs_lengths[i] > 0) {
            roaring_bitmap_add_range(rhs_bitmap, rhs_starts[i], rhs_starts[i] + rhs_lengths[i]);
        }
    }

    uint64_t result = roaring_bitmap_intersection_cardinality(lhs_bitmap, rhs_bitmap);

    roaring_bitmap_destroy(lhs_bitmap);
    roaring_bitmap_destroy(rhs_bitmap);

    return result;
}