#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {

    if (iters <= 0 || values.empty()) {
        return 0;
    }

    // If block_size is zero or no metadata, fallback to full scan
    if (block_size == 0 || mins.empty() || maxs.empty()) {
        uint64_t sum = 0;
        for (uint32_t v : values) {
            if (v >= low && v <= high) sum += static_cast<uint64_t>(v);
        }
        return sum;
    }

    const size_t n = values.size();
    const size_t num_groups = (n + block_size - 1) / block_size;

    uint64_t total = 0;

    for (size_t g = 0; g < num_groups; ++g) {
        uint32_t gmin = mins[g];
        uint32_t gmax = maxs[g];

        // Skip groups entirely outside the query range
        if (gmin > high || gmax < low) {
            continue;
        }

        size_t start = g * block_size;
        size_t end   = (start + block_size) < n ? (start + block_size) : n;

        // Group fully inside range: sum all values without condition
        if (gmin >= low && gmax <= high) {
            for (size_t i = start; i < end; ++i) {
                total += static_cast<uint64_t>(values[i]);
            }
        } else {
            // Partial overlap: test each value
            for (size_t i = start; i < end; ++i) {
                uint32_t v = values[i];
                if (v >= low && v <= high) {
                    total += static_cast<uint64_t>(v);
                }
            }
        }
    }

    return total;
}