#include "interface.h"
#include "roaring.h"

uint64_t bulk_add_cardinality(const std::vector<uint32_t>& values) {
    roaring_bitmap_t* rb = roaring_bitmap_create();
    if (!values.empty()) {
        roaring_bitmap_add_many(rb, values.size(), values.data());
    }
    uint64_t card = roaring_bitmap_get_cardinality(rb);
    roaring_bitmap_free(rb);
    return card;
}