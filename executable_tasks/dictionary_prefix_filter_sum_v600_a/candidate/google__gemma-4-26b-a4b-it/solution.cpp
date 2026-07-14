#include "interface.h"

#include <string>
#include <vector>

namespace {

// The prefix rule: s[0] == 'q' && s[1] <= 'm'
// We pre-calculate this for the dictionary to avoid repeated string access.
inline bool check_prefix(const std::string& s) {
    return s.size() >= 2 && s[0] == 'q' && s[1] <= 'm';
}

} // namespace

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    
    if (codes.empty()) return 0;

    // Pre-calculate which dictionary indices satisfy the prefix condition.
    // Using std::vector<char> as a fast boolean mask.
    std::vector<char> dict_matches(dictionary.size());
    for (std::size_t i = 0; i < dictionary.size(); ++i) {
        dict_matches[i] = check_prefix(dictionary[i]) ? 1 : 0;
    }

    uint64_t total_sum = 0;
    const std::size_t n = codes.size();
    const uint16_t* codes_ptr = codes.data();
    const uint32_t* weights_ptr = weights.data();
    const char* matches_ptr = dict_matches.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = 0;
        for (std::size_t i = 0; i < n; ++i) {
            // Look up the pre-calculated match status for the dictionary entry
            // pointed to by the current code.
            if (matches_ptr[codes_ptr[i]]) {
                current_iter_sum += weights_ptr[i];
            }
        }
        total_sum = current_iter_sum;
    }

    return total_sum;
}