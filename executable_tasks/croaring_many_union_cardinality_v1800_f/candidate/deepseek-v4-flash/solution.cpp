#include "interface.h"
#include <cstdint>
#include <vector>
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
    roaring_bitmap_t *r = roaring_bitmap_create();
    const size_t shard_count = offsets.size() - 1;
    for (size_t shard = 0; shard < shard_count; ++shard) {
        uint32_t begin = offsets[shard];
        uint32_t end = offsets[shard + 1];
        uint32_t len = end - begin;
        if (len > 0) {
            roaring_bitmap_add_many(r, len, &values[begin]);
        }
    }
    uint64_t card = roaring_bitmap_get_cardinality(r);
    roaring_bitmap_free(r);
    return card;
}
