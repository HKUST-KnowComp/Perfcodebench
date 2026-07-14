#include "interface.h"
#include "roaring.h"

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
    roaring::Roaring bitmap;
    for (std::size_t shard = 0; shard + 1 < offsets.size(); ++shard) {
        const uint32_t begin = offsets[shard];
        const uint32_t end = offsets[shard + 1];
        for (uint32_t i = begin; i < end; ++i) {
            bitmap.add(values[i]);
        }
    }
    return bitmap.cardinality();
}
