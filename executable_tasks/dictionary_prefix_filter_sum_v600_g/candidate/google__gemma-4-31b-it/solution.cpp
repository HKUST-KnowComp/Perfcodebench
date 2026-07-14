#include "interface.h"

#include <vector>
#include <string>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    
    if (codes.empty()) return 0;

    // Pre-calculate which dictionary entries match the prefix rule
    // Rule: s[0] == 'q' && s[1] <= 'm'
    std::vector<bool> dict_matches(dictionary.size());
    for (size_t i = 0; i < dictionary.size(); ++i) {
        const std::string& s = dictionary[i];
        if (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm') {
            dict_matches[i] = true;
        } else {
            dict_matches[i] = false;
        }
    }

    uint64_t final_sum = 0;
    size_t n = codes.size();
    
    // The problem asks for the sum for the last iteration (based on baseline behavior)
    // The baseline resets 'sum = 0' inside the loop, so we only care about the final result.
    // However, to be safe and follow the logic of the baseline exactly:
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i < n; ++i) {
            if (dict_matches[codes[i]]) {
                current_sum += weights[i];
            }
        }
        final_sum = current_sum;
    }

    return final_sum;
}