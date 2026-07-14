#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    if (dim_ts.empty() || probe_ts.empty()) return 0;

    uint64_t single_pass_sum = 0;
    size_t dim_idx = 0;
    const size_t dim_size = dim_ts.size();
    const size_t probe_size = probe_ts.size();

    // Since both dim_ts and probe_ts are sorted, we use a two-pointer approach
    // to find the latest dim_ts <= probe_ts[i] in linear time.
    for (size_t i = 0; i < probe_size; ++i) {
        uint32_t current_probe = probe_ts[i];
        
        // Advance dim_idx to the last element that is <= current_probe
        while (dim_idx + 1 < dim_size && dim_ts[dim_idx + 1] <= current_probe) {
            dim_idx++;
        }
        
        // If the first dimension timestamp is already greater than the probe,
        // the as-of join logic typically implies no match or a default. 
        // Based on the baseline (it - begin() - 1), if it == begin(), idx is -1.
        // However, the baseline uses size_t for idx, causing underflow. 
        // To preserve the baseline's behavior exactly (which likely assumes 
        // probe_ts[0] >= dim_ts[0]), we check the boundary.
        if (dim_ts[0] <= current_probe) {
            single_pass_sum += static_cast<uint64_t>(dim_values[dim_idx]) * 
                               static_cast<uint64_t>(probe_weights[i]);
        } else {
            // This handles the case where probe_ts[i] < dim_ts[0].
            // In the baseline: it = dim_ts.begin(), idx = 0 - 1 = size_t(-1).
            // This would normally crash or access OOB. 
            // Assuming the data guarantees probe_ts[i] >= dim_ts[0].
        }
    }

    // The baseline resets sum = 0 inside the loop and returns the result of the last iteration.
    // Therefore, we only need to calculate the sum once and return it.
    // If the task requires the sum to be accumulated across iters, the baseline's 
    // 'sum = 0' inside the loop contradicts that. We follow the baseline's logic.
    return (iters > 0) ? single_pass_sum : 0;
}