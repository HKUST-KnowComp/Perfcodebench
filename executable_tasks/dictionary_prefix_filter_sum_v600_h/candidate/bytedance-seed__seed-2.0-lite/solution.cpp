#include "interface.h"

#include <string>
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    // Precompute prefix match validity for all dictionary entries once, outside hot loop
    std::vector<bool> is_valid(dictionary.size(), false);
    for (size_t j = 0; j < dictionary.size(); ++j) {
        const std::string& s = dictionary[j];
        // Exact original match logic, safely avoids out-of-bounds string access
        if (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm') {
            is_valid[j] = true;
        }
    }

    uint64_t final_sum = 0;
    const size_t num_codes = codes.size();
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        // Optimized hot loop: sequential, cache-friendly, no string operations
        for (size_t i = 0; i < num_codes; ++i) {
            if (is_valid[static_cast<size_t>(codes[i])]) {
                sum += static_cast<uint64_t>(weights[i]);
            }
        }
        final_sum = sum; // Preserve original behavior of returning the last iteration's sum
    }
    return final_sum;
}