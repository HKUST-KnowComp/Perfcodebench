#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
    
    uint64_t sum = 0;
    size_t num_blocks = mins.size();
    const uint32_t* values_data = values.data();
    size_t values_size = values.size();
    
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t i = 0; i < num_blocks; ++i) {
            if (mins[i] > high || maxs[i] < low) {
                continue;
            }
            size_t start = i * block_size;
            size_t end = start + block_size;
            if (end > values_size) end = values_size;
            
            const uint32_t* ptr = values_data + start;
            const uint32_t* end_ptr = values_data + end;
            for (; ptr < end_ptr; ++ptr) {
                uint32_t v = *ptr;
                if (v >= low && v <= high) {
                    sum += v;
                }
            }
        }
    }
    return sum;
}