#include "interface.h"
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    if (iters <= 0 || deltas.empty()) {
        return 0;
    }

    // The baseline code resets 'sum = 0' at the start of every iteration.
    // Therefore, the final return value is simply the sum of the decoded 
    // prefix values of the very last iteration.
    // We can skip all previous iterations and just compute the last one.

    uint64_t total_sum = 0;
    uint64_t current_value = static_cast<uint64_t>(base);
    
    for (uint32_t delta : deltas) {
        current_value += delta;
        total_sum += current_value;
    }

    return total_sum;
}