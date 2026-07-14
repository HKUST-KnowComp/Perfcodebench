#include "interface.h"
#include "roaring.h"
#include <vector>
#include <cstdint>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
    if (offsets.size() < 2) return 0;

    roaring_bitmap_t* acc = roaring_bitmap_create();
    for (size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
        const uint32_t begin = offsets[shard];
        const uint32_t end   = offsets[shard + 1];
        const uint32_t len   = end - begin;
        if (len == 0) continue;

        roaring_bitmap_t* tmp = roaring_bitmap_of_ptr(len, &values[begin]);
        roaring_bitmap_or_inplace(acc, tmp);
        roaring_bitmap_free(tmp);
    }

    const uint64_t card = roaring_bitmap_get_cardinality(acc);
    roaring_bitmap_free(acc);
    return card;
}