#include "interface.h"
#include "roaring.h"

uint64_t difference_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
    
    roaring_bitmap_t* lhs = roaring_bitmap_create();
    roaring_bitmap_t* rhs = roaring_bitmap_create();

    for (size_t i = 0; i < lhs_starts.size(); ++i) {
        roaring_bitmap_add_range(lhs, lhs_starts[i], lhs_starts[i] + lhs_lengths[i]);
    }

    for (size_t i = 0; i < rhs_starts.size(); ++i) {
        roaring_bitmap_add_range(rhs, rhs_starts[i], rhs_starts[i] + rhs_lengths[i]);
    }

    roaring_bitmap_andnot(lhs, rhs);
    uint64_t result = roaring_bitmap_get_cardinality(lhs);

    roaring_bitmap_free(lhs);
    roaring_bitmap_free(rhs);

    return result;
}