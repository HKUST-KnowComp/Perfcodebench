#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
    roaring_bitmap_t* union_bitmap = roaring_bitmap_create();
    if (!union_bitmap) {
        return 0;
    }

    const size_t num_shards = offsets.size() > 0 ? offsets.size() - 1 : 0;
    for (size_t i = 0; i < num_shards; ++i) {
        const uint32_t start = offsets[i];
        const uint32_t end = offsets[i + 1];
        const size_t count = end - start;
        if (count == 0) {
            continue;
        }
        const uint32_t* data = values.data() + start;
        roaring_bitmap_add_many_sorted(union_bitmap, count, data);
    }

    const uint64_t result = roaring_bitmap_get_cardinality(union_bitmap);
    roaring_bitmap_free(union_bitmap);
    return result;
}