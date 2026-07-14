#include "interface.h"
#include "roaring.h"
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
    if (offsets.size() < 2) return 0;
    
    size_t num_shards = offsets.size() - 1;
    std::vector<roaring_bitmap_t*> bitmaps;
    bitmaps.reserve(num_shards);
    
    for (size_t i = 0; i < num_shards; ++i) {
        uint32_t begin = offsets[i];
        uint32_t end = offsets[i + 1];
        size_t len = static_cast<size_t>(end - begin);
        if (len > 0) {
            bitmaps.push_back(roaring_bitmap_from_sorted_array(&values[begin], len));
        }
    }
    
    if (bitmaps.empty()) return 0;
    
    std::vector<const roaring_bitmap_t*> const_bitmaps(bitmaps.begin(), bitmaps.end());
    roaring_bitmap_t* union_bm = roaring_bitmap_or_many(const_bitmaps.size(), const_bitmaps.data());
    uint64_t card = roaring_bitmap_get_cardinality(union_bm);
    roaring_bitmap_free(union_bm);
    
    for (roaring_bitmap_t* bm : bitmaps) {
        roaring_bitmap_free(bm);
    }
    
    return card;
}