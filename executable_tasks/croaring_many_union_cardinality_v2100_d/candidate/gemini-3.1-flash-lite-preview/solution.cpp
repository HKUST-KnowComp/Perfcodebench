#include "interface.h"
#include "roaring.h"
#include <vector>

uint64_t many_union_cardinality(const std::vector<uint32_t>& offsets, const std::vector<uint32_t>& values) {
    if (offsets.size() <= 1) return 0;

    size_t num_shards = offsets.size() - 1;
    std::vector<roaring_bitmap_t*> bitmaps(num_shards);

    for (size_t i = 0; i < num_shards; ++i) {
        uint32_t start = offsets[i];
        uint32_t end = offsets[i + 1];
        if (start < end) {
            bitmaps[i] = roaring_bitmap_from_range(0, 0, 1); // dummy init
            roaring_bitmap_free(bitmaps[i]);
            bitmaps[i] = roaring_bitmap_create();
            roaring_bitmap_add_many(bitmaps[i], end - start, &values[start]);
        } else {
            bitmaps[i] = roaring_bitmap_create();
        }
    }

    roaring_bitmap_t* result = roaring_bitmap_or_many(num_shards, bitmaps.data());
    uint64_t cardinality = roaring_bitmap_get_cardinality(result);

    roaring_bitmap_free(result);
    for (size_t i = 0; i < num_shards; ++i) {
        roaring_bitmap_free(bitmaps[i]);
    }

    return cardinality;
}