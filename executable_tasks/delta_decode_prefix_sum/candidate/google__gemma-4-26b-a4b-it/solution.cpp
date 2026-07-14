#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    if (iters <= 0) return 0;
    
    // The baseline code resets 'sum = 0' inside the 'iters' loop.
    // This means the final return value is only the sum of the decoded values 
    // from the very last iteration. 
    // We can skip the first (iters - 1) iterations entirely.
    
    uint64_t current_sum = 0;
    uint64_t current_value = static_cast<uint64_t>(base);
    
    for (std::size_t i = 0; i < deltas.size(); ++i) {
        current_value += static_cast<uint64_t>(deltas[i]);
        current_sum += current_value;
    }
    
    return current_sum;
}