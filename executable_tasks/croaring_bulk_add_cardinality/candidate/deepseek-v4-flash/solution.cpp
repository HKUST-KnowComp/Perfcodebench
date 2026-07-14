#include "interface.h"
#include "roaring.h"

uint64_t bulk_add_cardinality(const std::vector<uint32_t>& values) {
    roaring_bitmap_t *r = roaring_bitmap_create();
    roaring_bitmap_add_many(r, values.size(), values.data());
    uint64_t card = roaring_bitmap_get_cardinality(r);
    roaring_bitmap_free(r);
    return card;
}
