#include "interface.h"
#include "roaring.h"

uint64_t xor_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {

    roaring_bitmap_t* lhs_bm = roaring_bitmap_create();
    roaring_bitmap_t* rhs_bm = roaring_bitmap_create();

    for (size_t i = 0; i < lhs_starts.size(); ++i) {
        if (lhs_lengths[i] > 0) {
            roaring_bitmap_add_range_closed(lhs_bm, lhs_starts[i], lhs_starts[i] + lhs_lengths[i] - 1);
        }
    }

    for (size_t i = 0; i < rhs_starts.size(); ++i) {
        if (rhs_lengths[i] > 0) {
            roaring_bitmap_add_range_closed(rhs_bm, rhs_starts[i], rhs_starts[i] + rhs_lengths[i] - 1);
        }
    }

    uint64_t result = roaring_bitmap_xor_cardinality(lhs_bm, rhs_bm);

    roaring_bitmap_free(lhs_bm);
    roaring_bitmap_free(rhs_bm);

    return result;
}