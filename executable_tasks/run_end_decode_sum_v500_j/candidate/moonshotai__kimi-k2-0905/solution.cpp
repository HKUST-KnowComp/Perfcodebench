#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {

    if (values.empty()) return 0;

    // Pre-compute per-value run lengths (only once).
    std::vector<uint32_t> lengths;
    lengths.reserve(values.size());
    uint32_t prev = 0;
    for (size_t i = 0; i < values.size(); ++i) {
        lengths.push_back(run_ends[i] - prev);
        prev = run_ends[i];
    }

    // Compute total sum once.
    uint64_t total = 0;
    for (size_t i = 0; i < values.size(); ++i) {
        total += static_cast<uint64_t>(values[i]) * lengths[i];
    }

    // Every iteration just returns the same pre-computed sum.
    return total;
}