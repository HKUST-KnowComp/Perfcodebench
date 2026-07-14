#include "interface.h"
#include <roaring.h>

uint64_t difference_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
    
    roaring_bitmap_t* lhs = roaring_bitmap_create();
    roaring_bitmap_t* rhs = roaring_bitmap_create();
    
    // Add all LHS ranges
    for (std::size_t i = 0; i < lhs_starts.size(); ++i) {
        uint64_t start = lhs_starts[i];
        uint64_t end = start + lhs_lengths[i];
        if (end > start) {
            roaring_bitmap_add_range(lhs, start, end);
        }
    }
    
    // Add all RHS ranges
    for (std::size_t i = 0; i < rhs_starts.size(); ++i) {
        uint64_t start = rhs_starts[i];
        uint64_t end = start + rhs_lengths[i];
        if (end > start) {
            roaring_bitmap_add_range(rhs, start, end);
        }
    }
    
    // Compute difference in place: lhs = lhs AND NOT rhs
    roaring_bitmap_andnot_inplace(lhs, rhs);
    
    // Get cardinality
    uint64_t result = roaring_bitmap_get_cardinality(lhs);
    
    roaring_bitmap_free(lhs);
    roaring_bitmap_free(rhs);
    
    return result;
}