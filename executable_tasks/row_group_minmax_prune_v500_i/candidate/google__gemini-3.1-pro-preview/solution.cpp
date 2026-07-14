#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
    
    uint64_t total_sum = 0;
    const size_t num_values = values.size();
    const size_t num_blocks = mins.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = 0;
        
        for (size_t b = 0; b < num_blocks; ++b) {
            uint32_t b_min = mins[b];
            uint32_t b_max = maxs[b];

            // Case 1: Entire block is outside the range [low, high]
            if (b_min > high || b_max < low) {
                continue;
            }

            size_t start = b * block_size;
            size_t end = std::min(start + block_size, num_values);

            // Case 2: Entire block is inside the range [low, high]
            if (b_min >= low && b_max <= high) {
                uint64_t block_sum = 0;
                const uint32_t* ptr = &values[start];
                size_t count = end - start;
                
                // Unroll for performance
                size_t i = 0;
                for (; i + 3 < count; i += 4) {
                    block_sum += static_cast<uint64_t>(ptr[i]);
                    block_sum += static_cast<uint64_t>(ptr[i+1]);
                    block_sum += static_cast<uint64_t>(ptr[i+2]);
                    block_sum += static_cast<uint64_t>(ptr[i+3]);
                }
                for (; i < count; ++i) {
                    block_sum += static_cast<uint64_t>(ptr[i]);
                }
                current_iter_sum += block_sum;
            } 
            // Case 3: Partial overlap - filter elements
            else {
                uint64_t block_sum = 0;
                const uint32_t* ptr = &values[start];
                size_t count = end - start;
                
                size_t i = 0;
                for (; i + 3 < count; i += 4) {
                    uint32_t v0 = ptr[i];
                    uint32_t v1 = ptr[i+1];
                    uint32_t v2 = ptr[i+2];
                    uint32_t v3 = ptr[i+3];
                    
                    block_sum += (v0 >= low && v0 <= high) ? static_cast<uint64_t>(v0) : 0;
                    block_sum += (v1 >= low && v1 <= high) ? static_cast<uint64_t>(v1) : 0;
                    block_sum += (v2 >= low && v2 <= high) ? static_cast<uint64_t>(v2) : 0;
                    block_sum += (v3 >= low && v3 <= high) ? static_cast<uint64_t>(v3) : 0;
                }
                for (; i < count; ++i) {
                    uint32_t v = ptr[i];
                    if (v >= low && v <= high) {
                        block_sum += static_cast<uint64_t>(v);
                    }
                }
                current_iter_sum += block_sum;
            }
        }
        total_sum = current_iter_sum;
    }

    return total_sum;
}