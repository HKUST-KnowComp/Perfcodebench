#include "interface.h"
#include "roaring.h"
#include <vector>

uint64_t difference_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
    
    roaring_bitmap_t* lhs_bitmap = roaring_bitmap_create();
    roaring_bitmap_t* rhs_bitmap = roaring_bitmap_create();

    // Populate LHS bitmap using runs for efficiency
    for (std::size_t i = 0; i < lhs_starts.size(); ++i) {
        if (lhs_lengths[i] > 0) {
            roaring_bitmap_add_run(lhs_bitmap, lhs_starts[i], lhs_starts[i] + lhs_lengths[i]);
        }
    }

    // Populate RHS bitmap using runs for efficiency
    for (std::size_t i = 0; i < rhs_starts.size(); ++i) {
        if (rhs_lengths[i] > 0) {
            roaring_bitmap_add_run(rhs_bitmap, rhs_starts[i], rhs_starts[i] + rhs_lengths[i]);
        }
    }

    // Compute LHS AND NOT RHS
    // roaring_bitmap_and_not returns a new bitmap representing (lhs \ rhs)
    roaring_bitmap_t* diff_bitmap = roaring_bitmap_and_not(lhs_bitmap, rhs_bitmap);

    uint64_t result = roaring_bitmap_cardinality(diff_bitmap);

    // Cleanup
    roaring_bitmap_destroy(lhs_bitmap);
    roaring_bitmap_destroy(rhs_bitmap);
    roaring_bitmap_destroy(diff_bitmap);

    return result;
}