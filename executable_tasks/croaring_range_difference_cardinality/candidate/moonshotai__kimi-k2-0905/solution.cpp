#include "interface.h"
#include "roaring.h"

uint64_t difference_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
    roaring_bitmap_t* rb = roaring_bitmap_create();
    // Add all LHS ranges
    for (size_t i = 0; i < lhs_starts.size(); ++i) {
        uint32_t begin = lhs_starts[i];
        uint32_t end   = begin + lhs_lengths[i];
        if (end > begin) {
            roaring_bitmap_add_range(rb, begin, end);
        }
    }
    // Remove all RHS ranges
    for (size_t i = 0; i < rhs_starts.size(); ++i) {
        uint32_t begin = rhs_starts[i];
        uint32_t end   = begin + rhs_lengths[i];
        if (end > begin) {
            roaring_bitmap_remove_range(rb, begin, end);
        }
    }
    uint64_t card = roaring_bitmap_get_cardinality(rb);
    roaring_bitmap_free(rb);
    return card;
}