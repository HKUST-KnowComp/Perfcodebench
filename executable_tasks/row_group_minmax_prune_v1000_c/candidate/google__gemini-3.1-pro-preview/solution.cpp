#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
    
    size_t num_blocks = mins.size();
    size_t n = values.size();
    const uint32_t* val_ptr = values.data();
    const uint32_t* mins_ptr = mins.data();
    const uint32_t* maxs_ptr = maxs.data();
    
    std::vector<uint32_t> full_blocks;
    std::vector<uint32_t> partial_blocks;
    full_blocks.reserve(num_blocks);
    partial_blocks.reserve(num_blocks);
    
    for (size_t i = 0; i < num_blocks; ++i) {
        uint32_t b_min = mins_ptr[i];
        uint32_t b_max = maxs_ptr[i];
        if (b_max < low || b_min > high) {
            continue;
        } else if (b_min >= low && b_max <= high) {
            full_blocks.push_back(i);
        } else {
            partial_blocks.push_back(i);
        }
    }
    
    uint64_t final_sum = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        
        for (uint32_t i : full_blocks) {
            size_t start = static_cast<size_t>(i) * block_size;
            size_t end = start + block_size;
            if (end > n) end = n;
            
            uint64_t block_sum = 0;
            for (size_t j = start; j < end; ++j) {
                block_sum += val_ptr[j];
            }
            sum += block_sum;
        }
        
        for (uint32_t i : partial_blocks) {
            size_t start = static_cast<size_t>(i) * block_size;
            size_t end = start + block_size;
            if (end > n) end = n;
            
            uint64_t block_sum = 0;
            for (size_t j = start; j < end; ++j) {
                uint32_t v = val_ptr[j];
                if (v >= low && v <= high) {
                    block_sum += v;
                }
            }
            sum += block_sum;
        }
        
        final_sum = sum;
    }
    
    return final_sum;
}
