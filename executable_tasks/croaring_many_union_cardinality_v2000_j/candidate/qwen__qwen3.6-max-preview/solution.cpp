#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
    roaring_bitmap_t* rb = roaring_bitmap_create();
    for (std::size_t i = 0; i + 1 < offsets.size(); ++i) {
        uint32_t begin = offsets[i];
        uint32_t end = offsets[i + 1];
        if (end > begin) {
            roaring_bitmap_add_many(rb, end - begin, &values[begin]);
        }
    }
    uint64_t result = roaring_bitmap_get_cardinality(rb);
    roaring_bitmap_free(rb);
    return result;
}