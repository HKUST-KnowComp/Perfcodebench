#include "interface.h"
#include "roaring.h"

uint64_t xor_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
    roaring_bitmap_t *lhs = roaring_bitmap_create();
    roaring_bitmap_t *rhs = roaring_bitmap_create();

    for (size_t i = 0; i < lhs_starts.size(); ++i) {
        uint32_t start = lhs_starts[i];
        uint32_t length = lhs_lengths[i];
        if (length == 0 || start > UINT32_MAX - length) continue;
        roaring_bitmap_add_range(lhs, start, start + length);
    }

    for (size_t i = 0; i < rhs_starts.size(); ++i) {
        uint32_t start = rhs_starts[i];
        uint32_t length = rhs_lengths[i];
        if (length == 0 || start > UINT32_MAX - length) continue;
        roaring_bitmap_add_range(rhs, start, start + length);
    }

    uint64_t result = roaring_bitmap_xor_cardinality(lhs, rhs);
    roaring_bitmap_free(lhs);
    roaring_bitmap_free(rhs);

    return result;
}