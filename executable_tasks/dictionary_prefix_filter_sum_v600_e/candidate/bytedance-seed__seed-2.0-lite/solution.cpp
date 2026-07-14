#include "interface.h"

#include <string>
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    // Precompute validity of all dictionary entries once before iterations
    std::vector<bool> valid(dictionary.size());
    for (size_t i = 0; i < dictionary.size(); ++i) {
        const std::string& s = dictionary[i];
        valid[i] = (s[0] == 'q') && (s[1] <= 'm');
    }

    uint64_t final_sum = 0;
    const size_t num_rows = codes.size();
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        // Tight cache-friendly inner loop, no string operations
        for (size_t i = 0; i < num_rows; ++i) {
            if (valid[static_cast<size_t>(codes[i])]) {
                sum += static_cast<uint64_t>(weights[i]);
            }
        }
        final_sum = sum;
    }
    return final_sum;
}