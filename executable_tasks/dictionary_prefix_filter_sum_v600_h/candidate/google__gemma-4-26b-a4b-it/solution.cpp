#include "interface.h"
#include <string>
#include <vector>

namespace {

// Pre-calculate which dictionary indices satisfy the prefix condition.
// The condition is: s[0] == 'q' && s[1] <= 'm'
inline bool satisfies_prefix(const std::string& s) {
    if (s.size() < 2) return false;
    return s[0] == 'q' && s[1] <= 'm';
}

} // namespace

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    
    if (codes.empty()) return 0;

    // Step 1: Pre-calculate a boolean mask for the dictionary.
    // This avoids re-evaluating string logic inside the 'iters' loop.
    std::vector<bool> dict_matches(dictionary.size());
    for (std::size_t i = 0; i < dictionary.size(); ++i) {
        dict_matches[i] = satisfies_prefix(dictionary[i]);
    }

    uint64_t total_sum = 0;
    const std::size_t n = codes.size();
    const uint16_t* codes_ptr = codes.data();
    const uint32_t* weights_ptr = weights.data();
    const bool* mask_ptr = dict_matches.data();

    // Step 2: Perform the iterations using the pre-calculated mask.
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = 0;
        for (std::size_t i = 0; i < n; ++i) {
            // Check if the dictionary entry pointed to by the code matches the prefix
            if (mask_ptr[codes_ptr[i]]) {
                current_iter_sum += weights_ptr[i];
            }
        }
        total_sum = current_iter_sum;
    }

    return total_sum;
}