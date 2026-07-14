#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    
    const uint32_t* d_ts = dim_ts.data();
    const uint32_t* d_val = dim_values.data();
    const uint32_t* p_ts = probe_ts.data();
    const uint32_t* p_w = probe_weights.data();
    const size_t dim_size = dim_ts.size();
    const size_t probe_size = probe_ts.size();
    
    uint64_t sum = 0;
    if (dim_size == 0 || probe_size == 0) return sum;
    
    const uint32_t* d_ts_end = d_ts + dim_size;
    
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        const uint32_t* cur_d_ts = d_ts;
        const uint32_t* cur_d_val = d_val;
        const uint32_t* cur_p_ts = p_ts;
        const uint32_t* cur_p_w = p_w;
        for (size_t i = 0; i < probe_size; ++i) {
            uint32_t cur_p = *cur_p_ts++;
            while (cur_d_ts + 1 < d_ts_end && *(cur_d_ts + 1) <= cur_p) {
                ++cur_d_ts;
                ++cur_d_val;
            }
            sum += static_cast<uint64_t>(*cur_d_val) * *cur_p_w++;
        }
    }
    return sum;
}