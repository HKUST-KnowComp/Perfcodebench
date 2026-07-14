#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
    size_t num_blocks = values.size() / block_size;
    std::vector<size_t> included_blocks;
    included_blocks.reserve(num_blocks);
    for (size_t i = 0; i < num_blocks; ++i) {
        if (low <= maxs[i] && high >= mins[i]) {
            included_blocks.push_back(i);
        }
    }

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        const uint32_t* data = values.data();
        for (size_t idx : included_blocks) {
            const uint32_t* block = data + idx * block_size;
            const uint32_t* end = block + block_size;
            for (const uint32_t* p = block; p != end; ++p) {
                uint32_t v = *p;
                if (v >= low && v <= high) {
                    sum += v;
                }
            }
        }
    }
    return sum;
}