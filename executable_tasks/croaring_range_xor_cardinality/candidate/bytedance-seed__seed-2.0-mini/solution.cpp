#include "interface.h"
#include <roaring.h>

uint64_t xor_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
    roaring_bitmap_t* lhs_bm = roaring_bitmap_create();
    roaring_bitmap_t* rhs_bm = roaring_bitmap_create();

    for (size_t i = 0; i < lhs_starts.size(); ++i) {
        const uint32_t start = lhs_starts[i];
        const uint32_t end = start + lhs_lengths[i];
        roaring_bitmap_add_range(lhs_bm, start, end);
    }

    for (size_t i = 0; i < rhs_starts.size(); ++i) {
        const uint32_t start = rhs_starts[i];
        const uint32_t end = start + rhs_lengths[i];
        roaring_bitmap_add_range(rhs_bm, start, end);
    }

    roaring_bitmap_t* xor_bm = roaring_bitmap_xor(lhs_bm, rhs_bm);
    const uint64_t cardinality = roaring_bitmap_get_cardinality(xor_bm);

    roaring_bitmap_free(lhs_bm);
    roaring_bitmap_free(rhs_bm);
    roaring_bitmap_free(xor_bm);

    return cardinality;
}