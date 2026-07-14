#include "interface.h"
#include <roaring.h>

uint64_t xor_cardinality(
    const std::vector<uint32_t>& lhs_starts,
    const std::vector<uint32_t>& lhs_lengths,
    const std::vector<uint32_t>& rhs_starts,
    const std::vector<uint32_t>& rhs_lengths) {
    
    roaring_bitmap_t* lhs_bm = roaring_bitmap_create();
    roaring_bitmap_t* rhs_bm = roaring_bitmap_create();
    
    for (std::size_t i = 0; i < lhs_starts.size(); ++i) {
        roaring_bitmap_add_range(lhs_bm, lhs_starts[i], (uint64_t)lhs_starts[i] + lhs_lengths[i]);
    }
    for (std::size_t i = 0; i < rhs_starts.size(); ++i) {
        roaring_bitmap_add_range(rhs_bm, rhs_starts[i], (uint64_t)rhs_starts[i] + rhs_lengths[i]);
    }
    
    roaring_bitmap_run_optimize(lhs_bm);
    roaring_bitmap_run_optimize(rhs_bm);
    
    uint64_t result = roaring_bitmap_xor_cardinality(lhs_bm, rhs_bm);
    
    roaring_bitmap_free(lhs_bm);
    roaring_bitmap_free(rhs_bm);
    
    return result;
}
