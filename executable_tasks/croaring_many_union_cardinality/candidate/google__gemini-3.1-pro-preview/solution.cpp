#include "interface.h"
#include "roaring.h"
#include <vector>
#include <cstdint>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
    
    if (offsets.size() < 2) {
        return 0;
    }

    size_t num_shards = offsets.size() - 1;
    std::vector<const roaring_bitmap_t*> bitmaps;
    bitmaps.reserve(num_shards);

    // Create a roaring bitmap for each shard.
    // Since shards are sorted, roaring_bitmap_of_ptr is highly efficient.
    for (size_t i = 0; i < num_shards; ++i) {
        uint32_t start = offsets[i];
        uint32_t end = offsets[i + 1];
        uint32_t count = end - start;
        
        if (count > 0) {
            // roaring_bitmap_of_ptr expects a sorted array.
            bitmaps.push_back(roaring_bitmap_of_ptr(count, &values[start]));
        }
    }

    if (bitmaps.empty()) {
        return 0;
    }

    // Compute the union of all bitmaps efficiently using a heap-based merge.
    roaring_bitmap_t* union_bitmap = roaring_bitmap_or_many(bitmaps.size(), bitmaps.data());
    
    uint64_t cardinality = 0;
    if (union_bitmap) {
        cardinality = roaring_bitmap_get_cardinality(union_bitmap);
        roaring_bitmap_free(union_bitmap);
    }

    // Clean up individual shard bitmaps.
    for (const roaring_bitmap_t* b : bitmaps) {
        roaring_bitmap_free(const_cast<roaring_bitmap_t*>(b));
    }

    return cardinality;
}