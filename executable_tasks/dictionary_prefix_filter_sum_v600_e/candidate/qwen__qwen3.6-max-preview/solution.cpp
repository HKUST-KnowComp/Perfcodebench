#include "interface.h"
#include <vector>
#include <string>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    
    if (iters <= 0) return 0;

    // Precompute match status for each dictionary entry to avoid repeated string checks
    std::vector<uint8_t> match_table(dictionary.size());
    for (size_t i = 0; i < dictionary.size(); ++i) {
        const std::string& s = dictionary[i];
        match_table[i] = (s[0] == 'q' && s[1] <= 'm') ? 1 : 0;
    }

    uint64_t sum = 0;
    const uint16_t* c = codes.data();
    const uint32_t* w = weights.data();
    size_t n = codes.size();
    
    // Tight loop over codes/weights using the precomputed lookup table
    for (size_t i = 0; i < n; ++i) {
        sum += static_cast<uint64_t>(match_table[c[i]]) * w[i];
    }

    // The operation is idempotent across iterations, so multiply once
    return sum * static_cast<uint64_t>(iters);
}