#include "interface.h"
#include <roaring.h>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
    if (offsets.size() < 2) {
        return 0;
    }
    roaring_bitmap_t *main = nullptr;
    for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
        const uint32_t begin = offsets[shard];
        const uint32_t end = offsets[shard + 1];
        if (begin >= end) continue;
        if (!main) {
            main = roaring_bitmap_from_sorted_array(values.data() + begin, end - begin);
        } else {
            roaring_bitmap_t *shard_bm = roaring_bitmap_from_sorted_array(values.data() + begin, end - begin);
            roaring_bitmap_or_inplace(main, shard_bm);
            roaring_bitmap_free(shard_bm);
        }
    }
    if (!main) {
        return 0;
    }
    uint64_t card = roaring_bitmap_get_cardinality(main);
    roaring_bitmap_free(main);
    return card;
}