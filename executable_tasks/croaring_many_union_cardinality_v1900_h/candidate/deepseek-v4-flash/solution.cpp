#include "interface.h"
#include <roaring.h>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
    roaring_bitmap_t* r = roaring_bitmap_create();
    if (offsets.size() < 2) {
        roaring_bitmap_free(r);
        return 0;
    }
    for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
        const uint32_t begin = offsets[shard];
        const uint32_t end = offsets[shard + 1];
        if (begin < end) {
            const uint32_t* data = values.data() + begin;
            const std::size_t count = end - begin;
            roaring_bitmap_add_many(r, count, data);
        }
    }
    const uint64_t cardinality = roaring_bitmap_get_cardinality(r);
    roaring_bitmap_free(r);
    return cardinality;
}
