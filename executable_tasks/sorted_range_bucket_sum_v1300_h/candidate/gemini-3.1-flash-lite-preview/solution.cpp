#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t range_bucket_sum(const std::vector<uint32_t>& boundaries, const std::vector<uint32_t>& probes, int iters) {
    uint64_t total_sum = 0;
    const uint32_t* b_ptr = boundaries.data();
    const size_t b_size = boundaries.size();
    const uint32_t* p_ptr = probes.data();
    const size_t p_size = probes.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        size_t b_idx = 0;
        for (size_t p_idx = 0; p_idx < p_size; ++p_idx) {
            uint32_t probe = p_ptr[p_idx];
            while (b_idx < b_size && b_ptr[b_idx] <= probe) {
                b_idx++;
            }
            current_sum += b_idx;
        }
        total_sum = current_sum;
    }
    return total_sum;
}