#include "interface.h"
#include <roaring.h>

uint64_t union_cardinality(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& lengths) {
    roaring_bitmap_t* bitmap = roaring_bitmap_create();
    for (size_t i = 0; i < starts.size(); ++i) {
        uint64_t start = starts[i];
        uint64_t end = start + lengths[i];
        roaring_bitmap_add_range(bitmap, start, end);
    }
    uint64_t cardinality = roaring_bitmap_get_cardinality(bitmap);
    roaring_bitmap_free(bitmap);
    return cardinality;
}
