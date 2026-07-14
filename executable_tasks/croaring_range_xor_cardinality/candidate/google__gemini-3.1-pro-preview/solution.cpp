#include "interface.h"
#include "roaring.h"
#include <cstdint>
#include <vector>

uint64_t xor_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
    
    // Initialize Roaring bitmaps
    roaring_bitmap_t* lhs_bm = roaring_bitmap_create();
    roaring_bitmap_t* rhs_bm = roaring_bitmap_create();

    // Add ranges to the LHS bitmap
    // roaring_bitmap_add_range uses [min, max) interval
    for (std::size_t i = 0; i < lhs_starts.size(); ++i) {
        uint64_t start = lhs_starts[i];
        uint64_t end = start + lhs_lengths[i];
        roaring_bitmap_add_range(lhs_bm, start, end);
    }

    // Add ranges to the RHS bitmap
    for (std::size_t i = 0; i < rhs_starts.size(); ++i) {
        uint64_t start = rhs_starts[i];
        uint64_t end = start + rhs_lengths[i];
        roaring_bitmap_add_range(rhs_bm, start, end);
    }

    // Compute the cardinality of the symmetric difference (XOR)
    // roaring_bitmap_xor_cardinality is more efficient than creating a new bitmap
    uint64_t result = roaring_bitmap_xor_cardinality(lhs_bm, rhs_bm);

    // Cleanup
    roaring_bitmap_free(lhs_bm);
    roaring_bitmap_free(rhs_bm);

    return result;
}