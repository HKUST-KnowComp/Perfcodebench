#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
    if (offsets.empty()) return 0;

    croaring::Roaring bitmap;
    
    // Iterate through each shard defined by the offsets
    for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
        const uint32_t begin = offsets[shard];
        const uint32_t end = offsets[shard + 1];
        
        // Since the input shards are sorted, we can use the optimized 
        // add_sorted method if available, or simply add the range.
        // CRoaring's add method is highly optimized for bulk insertions.
        if (begin < end) {
            bitmap.add_sorted(values.data() + begin, end - begin);
        }
    }
    
    return bitmap.cardinality();
}