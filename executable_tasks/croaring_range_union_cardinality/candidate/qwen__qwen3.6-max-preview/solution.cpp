#include "interface.h"
#include <roaring.h>

uint64_t union_cardinality(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& lengths) {
    roaring_bitmap_t* bitmap = roaring_bitmap_create();
    for (std::size_t i = 0; i < starts.size(); ++i) {
        uint32_t begin = starts[i];
        uint32_t end = begin + lengths[i];
        if (end > begin) {
            roaring_bitmap_add_range(bitmap, begin, end);
        }
    }
    uint64_t card = roaring_bitmap_get_cardinality(bitmap);
    roaring_bitmap_free(bitmap);
    return card;
}
