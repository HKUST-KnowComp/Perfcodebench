#include "interface.h"
#include <roaring.h>

uint64_t bulk_add_cardinality(const std::vector<uint32_t>& values) {
    if (values.empty()) return 0;
    roaring_bitmap_t* rb = roaring_bitmap_of_ptr(values.size(), values.data());
    uint64_t card = roaring_bitmap_get_cardinality(rb);
    roaring_bitmap_free(rb);
    return card;
}
