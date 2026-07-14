#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
    roaring_bitmap_t* unique_set = roaring_bitmap_create();
    if (!unique_set) return 0;

    const size_t num_shards = offsets.size() > 1 ? offsets.size() - 1 : 0;
    for (size_t shard = 0; shard < num_shards; ++shard) {
        const uint32_t begin = offsets[shard];
        const uint32_t end = offsets[shard + 1];
        const uint32_t count = end - begin;
        if (count == 0) continue;
        roaring_bitmap_add_many(unique_set, count, values.data() + begin);
    }

    const uint64_t result = roaring_bitmap_get_cardinality(unique_set);
    roaring_bitmap_free(unique_set);
    return result;
}