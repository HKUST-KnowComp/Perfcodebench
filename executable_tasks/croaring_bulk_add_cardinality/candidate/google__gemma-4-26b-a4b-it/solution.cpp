#include "interface.h"
#include "roaring.h"
#include <vector>

uint64_t bulk_add_cardinality(const std::vector<uint32_t>& values) {
    if (values.empty()) {
        return 0;
    }

    roaring_bitmap_t* bitmap = roaring_bitmap_create();
    if (!bitmap) {
        return 0;
    }

    // roaring_bitmap_add_many is optimized for bulk insertion
    // It handles the internal container management more efficiently than individual adds
    roaring_bitmap_add_many(bitmap, values.data(), static_cast<uint64_t>(values.size()));

    uint64_t cardinality = roaring_bitmap_get_cardinality(bitmap);
    
    roaring_bitmap_destroy(bitmap);
    return cardinality;
}