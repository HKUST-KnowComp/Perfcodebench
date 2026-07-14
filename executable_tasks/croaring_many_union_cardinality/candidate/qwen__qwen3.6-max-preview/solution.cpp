#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
    roaring_bitmap_t* r = roaring_bitmap_create();
    for (std::size_t i = 0; i + 1 < offsets.size(); ++i) {
        uint32_t begin = offsets[i];
        uint32_t end = offsets[i + 1];
        if (end > begin) {
            roaring_bitmap_add_many(r, static_cast<size_t>(end - begin), values.data() + begin);
        }
    }
    uint64_t card = roaring_bitmap_get_cardinality(r);
    roaring_bitmap_free(r);
    return card;
}