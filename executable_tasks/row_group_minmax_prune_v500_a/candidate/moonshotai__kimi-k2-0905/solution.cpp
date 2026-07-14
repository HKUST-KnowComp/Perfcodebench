#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {

    const size_t num_blocks = mins.size();
    std::vector<uint32_t> block_start(num_blocks);
    std::vector<uint32_t> block_end(num_blocks);
    std::vector<uint64_t> block_sum(num_blocks, 0);

    /* ---------- one-time preprocessing ---------- */
    size_t offset = 0;
    for (size_t b = 0; b < num_blocks; ++b) {
        const uint32_t mn = mins[b];
        const uint32_t mx = maxs[b];

        if (mx < low || mn > high) {
            /* entire block can be skipped */
            block_start[b] = block_end[b] = 0;   /* mark empty */
        } else {
            /* block may contain qualifying values – compute its prefix */
            const size_t start = offset;
            const size_t end   = offset + block_size;
            uint64_t local_sum = 0;
            for (size_t i = start; i < end; ++i) {
                const uint32_t v = values[i];
                local_sum += (v >= low && v <= high) ? static_cast<uint64_t>(v) : 0;
            }
            block_sum[b]   = local_sum;
            block_start[b] = static_cast<uint32_t>(start);
            block_end[b]   = static_cast<uint32_t>(end);
        }
        offset += block_size;
    }

    /* ---------- repeated fast aggregation ---------- */
    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_sum = 0;
        for (size_t b = 0; b < num_blocks; ++b) {
            if (block_end[b] > block_start[b]) {   /* non-empty */
                iter_sum += block_sum[b];
            }
        }
        total = iter_sum;   // last iter wins
    }
    return total;
}