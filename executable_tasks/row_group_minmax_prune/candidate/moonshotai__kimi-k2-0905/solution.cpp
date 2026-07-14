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

    const uint32_t n = static_cast<uint32_t>(values.size());
    const uint32_t num_blocks = (n + block_size - 1) / block_size;

    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        uint32_t idx = 0;

        for (uint32_t b = 0; b < num_blocks; ++b) {
            const uint32_t block_min = mins[b];
            const uint32_t block_max = maxs[b];

            // Skip block if it cannot contribute
            if (block_min > high || block_max < low) {
                idx += block_size;
                continue;
            }

            const uint32_t block_end = (b + 1 == num_blocks) ? n : idx + block_size;

            // Fast path: whole block in range
            if (block_min >= low && block_max <= high) {
                // Unroll 8× to hide latency
                const uint32_t* ptr = &values[idx];
                uint32_t j = idx;
                uint64_t local = 0;
                for (; j + 8 <= block_end; j += 8) {
                    local += ptr[0];
                    local += ptr[1];
                    local += ptr[2];
                    local += ptr[3];
                    local += ptr[4];
                    local += ptr[5];
                    local += ptr[6];
                    local += ptr[7];
                    ptr += 8;
                }
                for (; j < block_end; ++j) {
                    local += values[j];
                }
                sum += local;
                idx = block_end;
                continue;
            }

            // Partial block: check each value
            for (; idx < block_end; ++idx) {
                const uint32_t v = values[idx];
                if (v >= low && v <= high) {
                    sum += v;
                }
            }
        }

        total = sum;
    }
    return total;
}