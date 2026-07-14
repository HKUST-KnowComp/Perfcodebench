#include "interface.h"
#include "roaring.h"
#include <cstdint>
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
    
    if (offsets.size() < 2) {
        return 0;
    }

    // Create a new roaring bitmap
    roaring_bitmap_t *bm = roaring_bitmap_create();
    if (!bm) return 0;

    // Iterate through each shard defined by the offsets.
    // Since each shard is sorted, roaring_bitmap_add_many is highly efficient.
    for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
        const uint32_t begin = offsets[shard];
        const uint32_t end = offsets[shard + 1];
        
        if (end > begin) {
            // roaring_bitmap_add_many is optimized for sorted input arrays.
            roaring_bitmap_add_many(bm, (size_t)(end - begin), &values[begin]);
        }
    }

    // Get the final cardinality of the union.
    uint64_t cardinality = roaring_bitmap_get_cardinality(bm);

    // Clean up resources.
    roaring_bitmap_free(bm);

    return cardinality;
}