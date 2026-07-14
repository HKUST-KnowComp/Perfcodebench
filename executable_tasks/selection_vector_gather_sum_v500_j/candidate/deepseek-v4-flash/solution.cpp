#include "interface.h"
#include <vector>
#include <cstddef>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
    std::size_t n = keys.size();
    
    // Precompute indices that satisfy the predicate
    std::vector<std::size_t> selected;
    selected.reserve(n);
    for (std::size_t i = 0; i < n; ++i) {
        if (flags[i] != 0 && keys[i] < threshold && ((keys[i] & 7u) == 0u)) {
            selected.push_back(i);
        }
    }
    
    uint64_t sum = 0;
    const uint32_t* payload_ptr = payloads.data();
    std::size_t selected_count = selected.size();
    
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (std::size_t j = 0; j < selected_count; ++j) {
            sum += static_cast<uint64_t>(payload_ptr[selected[j]]);
        }
    }
    return sum;
}
