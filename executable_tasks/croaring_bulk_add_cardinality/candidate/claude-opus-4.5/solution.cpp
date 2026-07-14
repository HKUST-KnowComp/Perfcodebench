#include "interface.h"

#include <roaring.h>

uint64_t bulk_add_cardinality(const std::vector<uint32_t>& values) {
    if (values.empty()) {
        return 0;
    }
    
    roaring_bitmap_t *r = roaring_bitmap_create();
    
    // Use bulk add - CRoaring is optimized for sorted input
    roaring_bitmap_add_many(r, values.size(), values.data());
    
    // Run optimize to compress and improve performance
    roaring_bitmap_run_optimize(r);
    
    uint64_t cardinality = roaring_bitmap_get_cardinality(r);
    
    roaring_bitmap_free(r);
    
    return cardinality;
}