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
    
    const size_t n = values.size();
    const size_t num_blocks = mins.size();
    
    if (n == 0 || num_blocks == 0 || block_size == 0) {
        return 0;
    }
    
    // Precompute block sums for fully-included blocks
    std::vector<uint64_t> block_sums(num_blocks, 0);
    for (size_t b = 0; b < num_blocks; ++b) {
        size_t start = b * block_size;
        size_t end = start + block_size;
        if (end > n) end = n;
        uint64_t s = 0;
        for (size_t i = start; i < end; ++i) {
            s += values[i];
        }
        block_sums[b] = s;
    }
    
    uint64_t sum = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        
        for (size_t b = 0; b < num_blocks; ++b) {
            uint32_t block_min = mins[b];
            uint32_t block_max = maxs[b];
            
            // Skip: block entirely outside range
            if (block_max < low || block_min > high) {
                continue;
            }
            
            size_t start = b * block_size;
            size_t end = start + block_size;
            if (end > n) end = n;
            
            // Fully include: block entirely inside range
            if (block_min >= low && block_max <= high) {
                sum += block_sums[b];
                continue;
            }
            
            // Partial scan: block overlaps range boundary
            for (size_t i = start; i < end; ++i) {
                uint32_t v = values[i];
                if (v >= low && v <= high) {
                    sum += v;
                }
            }
        }
    }
    
    return sum;
}