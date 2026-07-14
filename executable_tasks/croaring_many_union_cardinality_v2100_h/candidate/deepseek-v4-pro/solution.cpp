#include "interface.h"
#include "roaring.h"
#include <cstdint>
#include <vector>

uint64_t many_union_cardinality(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& values) {
    const size_t num_shards = offsets.empty() ? 0 : offsets.size() - 1;
    if (num_shards == 0) return 0ULL;

    std::vector<roaring_bitmap_t*> bms;
    bms.reserve(num_shards);

    for (size_t s = 0; s < num_shards; ++s) {
        const uint32_t begin = offsets[s];
        const uint32_t end = offsets[s+1];
        if (begin < end) {
            bms.push_back(roaring_bitmap_of_ptr(end - begin, values.data() + begin));
        }
    }

    if (bms.empty()) return 0ULL;
    if (bms.size() == 1) {
        uint64_t card = roaring_bitmap_get_cardinality(bms[0]);
        roaring_bitmap_free(bms[0]);
        return card;
    }

    roaring_bitmap_t* result = roaring_bitmap_or_many_heap(bms.size(), bms.data());
    uint64_t card = roaring_bitmap_get_cardinality(result);
    roaring_bitmap_free(result);
    for (auto bm : bms) roaring_bitmap_free(bm);
    return card;
}