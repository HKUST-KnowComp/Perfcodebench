#include "interface.h"
#include <roaring.h>
#include <vector>
#include <cstdint>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
    
    if (offsets.size() <= 1) {
        return 0;
    }
    
    const size_t num_shards = offsets.size() - 1;
    
    // Build one roaring bitmap per shard
    std::vector<roaring_bitmap_t*> bitmaps(num_shards);
    
    #pragma omp parallel for schedule(dynamic)
    for (size_t shard = 0; shard < num_shards; ++shard) {
        const uint32_t begin = offsets[shard];
        const uint32_t end = offsets[shard + 1];
        const uint32_t count = end - begin;
        
        // Create bitmap from sorted array
        bitmaps[shard] = roaring_bitmap_of_ptr(count, values.data() + begin);
        roaring_bitmap_run_optimize(bitmaps[shard]);
    }
    
    // Use heap-based many-way OR for efficiency
    roaring_bitmap_t* result = roaring_bitmap_or_many_heap(
        static_cast<uint32_t>(num_shards),
        const_cast<const roaring_bitmap_t**>(bitmaps.data()));
    
    uint64_t cardinality = roaring_bitmap_get_cardinality(result);
    
    // Cleanup
    roaring_bitmap_free(result);
    for (size_t i = 0; i < num_shards; ++i) {
        roaring_bitmap_free(bitmaps[i]);
    }
    
    return cardinality;
}