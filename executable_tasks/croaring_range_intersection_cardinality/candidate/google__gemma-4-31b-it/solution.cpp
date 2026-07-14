#include "interface.h"
#include "roaring.h"

uint64_t intersection_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
    
    roaring_bitmap_t* lhs = roaring_bitmap_create();
    roaring_bitmap_t* rhs = roaring_bitmap_create();

    for (size_t i = 0; i < lhs_starts.size(); ++i) {
        uint32_t start = lhs_starts[i];
        uint32_t end = start + lhs_lengths[i];
        // roaring_bitmap_add_range adds [start, end)
        roaring_bitmap_add_range(lhs, start, end);
    }

    for (size_t i = 0; i < rhs_starts.size(); ++i) {
        uint32_t start = rhs_starts[i];
        uint32_t end = start + rhs_lengths[i];
        roaring_bitmap_add_range(rhs, start, end);
    }

    uint64_t cardinality = roaring_bitmap_and_cardinality(lhs, rhs);

    roaring_bitmap_destroy(lhs);
    roaring_bitmap_destroy(rhs);

    return cardinality;
}