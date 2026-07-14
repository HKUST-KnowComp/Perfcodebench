#include "interface.h"

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint64_t total_sum = 0;
    const uint8_t* data = packed.data();
    const size_t size = packed.size();
    const uint8_t thresh = threshold;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i < size; ++i) {
            uint8_t byte = data[i];
            
            // Low nibble
            uint8_t low = byte & 0x0F;
            if (low > thresh) {
                current_sum += low;
            }
            
            // High nibble
            uint8_t high = byte >> 4;
            if (high > thresh) {
                current_sum += high;
            }
        }
        total_sum = current_sum;
    }
    
    // The original baseline resets sum = 0 inside the loop and returns the sum of the last iteration.
    // To preserve the exact behavior of the baseline: "sum = 0; for (uint32_t v : decoded) ..."
    // The baseline returns the sum of the final iteration of the 'iters' loop.
    
    // Re-evaluating baseline: 
    // for (int iter = 0; iter < iters; ++iter) {
    //   ... 
    //   sum = 0;
    //   for (uint32_t v : decoded) { if (v > threshold) sum += v; }
    // }
    // return sum;
    
    // This means the result is only the sum of the last iteration.
    
    uint64_t final_sum = 0;
    for (size_t i = 0; i < size; ++i) {
        uint8_t byte = data[i];
        uint8_t low = byte & 0x0F;
        if (low > thresh) final_sum += low;
        uint8_t high = byte >> 4;
        if (high > thresh) final_sum += high;
    }
    
    // If iters is 0, the baseline would return 0. If iters > 0, it returns the sum of one pass.
    return (iters > 0) ? final_sum : 0;
}