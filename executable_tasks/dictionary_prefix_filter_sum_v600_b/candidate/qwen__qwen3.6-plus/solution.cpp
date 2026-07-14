#include "interface.h"
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    
    // Precompute match status for each dictionary entry once
    std::vector<char> dict_match(dictionary.size(), 0);
    for (size_t i = 0; i < dictionary.size(); ++i) {
        const auto& s = dictionary[i];
        if (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm') {
            dict_match[i] = 1;
        }
    }
    
    uint64_t sum = 0;
    const size_t n = codes.size();
    const uint16_t* codes_ptr = codes.data();
    const uint32_t* weights_ptr = weights.data();
    const char* match_ptr = dict_match.data();
    
    // Compute sum. Baseline recomputes 'iters' times but returns the last result.
    // Since inputs are constant, one pass yields the identical correct result.
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t i = 0; i < n; ++i) {
            if (match_ptr[codes_ptr[i]]) {
                sum += static_cast<uint64_t>(weights_ptr[i]);
            }
        }
    }
    
    return sum;
}
