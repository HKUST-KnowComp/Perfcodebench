#include "interface.h"

#include <string>
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    // Precompute prefix match status for all dictionary entries once (one-time cost)
    std::vector<bool> is_valid(dictionary.size(), false);
    for (size_t d_idx = 0; d_idx < dictionary.size(); ++d_idx) {
        const std::string& s = dictionary[d_idx];
        // Avoid out-of-bounds access for strings shorter than 2 characters
        if (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm') {
            is_valid[d_idx] = true;
        }
    }

    uint64_t final_sum = 0;
    // Maintain iteration count to preserve benchmark timing requirements
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_sum = 0;
        // Hot path: cache-friendly loop with no string operations
        for (size_t i = 0; i < codes.size(); ++i) {
            if (is_valid[static_cast<size_t>(codes[i])]) {
                iter_sum += static_cast<uint64_t>(weights[i]);
            }
        }
        // Match original behavior of returning the final iteration's sum
        final_sum = iter_sum;
    }

    return final_sum;
}