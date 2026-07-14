#include "interface.h"
#include <roaring.h>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
    roaring_bitmap_t *ans = roaring_bitmap_create();
    if (!ans) {
        return 0;
    }
    for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
        const uint32_t begin = offsets[shard];
        const uint32_t end = offsets[shard + 1];
        if (end > begin) {
            roaring_bitmap_add_many(ans, static_cast<size_t>(end - begin),
                                    &values[begin]);
        }
    }
    uint64_t card = roaring_bitmap_get_cardinality(ans);
    roaring_bitmap_free(ans);
    return card;
}
