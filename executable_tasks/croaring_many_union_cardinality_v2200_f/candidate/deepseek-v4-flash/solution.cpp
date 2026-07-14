#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
    const size_t num_shards = offsets.size() - 1;
    if (num_shards == 0) {
        return 0;
    }

    std::vector<const roaring_bitmap_t*> bitmaps;
    bitmaps.reserve(num_shards);

    for (size_t s = 0; s < num_shards; ++s) {
        const uint32_t begin = offsets[s];
        const uint32_t end = offsets[s + 1];
        const size_t len = end - begin;
        if (len > 0) {
            // Shards are sorted, use efficient constructor
            roaring_bitmap_t* bm = roaring_bitmap_of_ptr(len, values.data() + begin);
            bitmaps.push_back(bm);
        }
    }

    if (bitmaps.empty()) {
        return 0;
    }

    // Union all bitmaps at once (lazy evaluation, efficient)
    roaring_bitmap_t* result = roaring_bitmap_or_many(
        bitmaps.size(),
        reinterpret_cast<const roaring_bitmap_t**>(bitmaps.data()));
    uint64_t cardinality = roaring_bitmap_get_cardinality(result);

    // Clean up
    roaring_bitmap_free(result);
    for (const auto* bm : bitmaps) {
        roaring_bitmap_free(const_cast<roaring_bitmap_t*>(bm));
    }

    return cardinality;
}
