#include "interface.h"
#include "roaring.h"
#include <vector>

uint64_t xor_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
    
    roaring_bitmap_t* lhs_bitmap = roaring_bitmap_create();
    roaring_bitmap_t* rhs_bitmap = roaring_bitmap_create();

    for (std::size_t i = 0; i < lhs_starts.size(); ++i) {
        uint32_t start = lhs_starts[i];
        uint32_t length = lhs_lengths[i];
        if (length > 0) {
            // roaring_bitmap_add_range is inclusive of start, exclusive of end
            roaring_bitmap_add_range(lhs_bitmap, start, start + length);
        }
    }

    for (std::size_t i = 0; i < rhs_starts.size(); ++i) {
        uint32_t start = rhs_starts[i];
        uint32_t length = rhs_lengths[i];
        if (length > 0) {
            roaring_bitmap_add_range(rhs_bitmap, start, start + length);
        }
    }

    // Symmetric difference: (A \ B) U (B \ A)
    // In Roaring, we can compute the XOR directly or use symmetric_difference
    // roaring_bitmap_symmetric_difference is available in many versions, 
    // but the most robust way to get cardinality of XOR is to use the XOR operation.
    
    roaring_bitmap_t* xor_bitmap = roaring_bitmap_create();
    roaring_bitmap_symmetric_difference(xor_bitmap, lhs_bitmap, rhs_bitmap);
    
    uint64_t result = roaring_bitmap_cardinality(xor_bitmap);

    roaring_bitmap_destroy(lhs_bitmap);
    roaring_bitmap_destroy(rhs_bitmap);
    roaring_bitmap_destroy(xor_bitmap);

    return result;
}