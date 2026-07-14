#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(const std::vector<uint32_t>& offsets,
                                const std::vector<uint32_t>& values) {
    if (offsets.size() < 2) return 0;

    roaring_bitmap_t *r = roaring_bitmap_create();
    if (!r) return 0;

    // Pre-allocate containers for the worst case (2^16 containers).
    roaring_bitmap_set_copy_on_write(r, false);

    for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
        const uint32_t begin = offsets[shard];
        const uint32_t end   = offsets[shard + 1];
        if (begin < end) {
            roaring_bitmap_add_range_closed(r, values[begin], values[end - 1]);
        }
    }

    const uint64_t cardinality = roaring_bitmap_get_cardinality(r);
    roaring_bitmap_free(r);
    return cardinality;
}