#include "interface.h"
#include "roaring.h"
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
    
    size_t num_shards = offsets.size() > 0 ? offsets.size() - 1 : 0;
    if (num_shards == 0) return 0;

    std::vector<const roaring_bitmap_t*> bitmaps(num_shards);
    for (size_t i = 0; i < num_shards; ++i) {
        uint32_t begin = offsets[i];
        uint32_t end = offsets[i+1];
        size_t count = end - begin;
        roaring_bitmap_t* bm = roaring_bitmap_create();
        if (count > 0) {
            roaring_bitmap_add_many(bm, count, &values[begin]);
        }
        bitmaps[i] = bm;
    }

    roaring_bitmap_t* result = roaring_bitmap_or_many(num_shards, bitmaps.data());
    uint64_t card = roaring_bitmap_get_cardinality(result);
    
    roaring_bitmap_free(result);
    for (size_t i = 0; i < num_shards; ++i) {
        roaring_bitmap_free(const_cast<roaring_bitmap_t*>(bitmaps[i]));
    }
    
    return card;
}