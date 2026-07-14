#include "interface.h"
#include "roaring.h"
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
    
    roaring_bitmap_t* rb = roaring_bitmap_create();
    size_t num_shards = offsets.size() > 0 ? offsets.size() - 1 : 0;
    for (size_t i = 0; i < num_shards; ++i) {
        uint32_t begin = offsets[i];
        uint32_t end = offsets[i + 1];
        uint32_t count = end - begin;
        if (count > 0) {
            roaring_bitmap_add_many(rb, count, &values[begin]);
        }
    }
    uint64_t result = roaring_bitmap_get_cardinality(rb);
    roaring_bitmap_free(rb);
    return result;
}