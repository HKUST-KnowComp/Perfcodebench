#include "interface.h"
#include <roaring/roaring.h>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values)
{
    roaring_bitmap_t* r = roaring_bitmap_create();
    if (!r) return 0;

    for (size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
        const uint32_t* begin = &values[offsets[shard]];
        const uint32_t* end   = &values[offsets[shard + 1]];
        roaring_bitmap_add_many(r, end - begin, begin);
    }

    const uint64_t card = roaring_bitmap_get_cardinality(r);
    roaring_bitmap_free(r);
    return card;
}