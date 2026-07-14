#include "interface.h"
#include <string>
#include <vector>

namespace {

// The prefix rule: s[0] == 'q' and s[1] <= 'm'
// We must ensure the string has at least 2 characters to avoid out-of-bounds access.
inline bool matches_prefix(const std::string& s) {
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
    // This avoids repeated string access and logic evaluation inside the loop.
    std::vector<bool> dict_matches(dictionary.size());
    for (std::size_t i = 0; i < dictionary.size(); ++i) {
        dict_matches[i] = matches_prefix(dictionary[i]);
    }

    uint64_t total_sum = 0;
    const std::size_t n = codes.size();
    const uint16_t* codes_ptr = codes.data();
    const uint32_t* weights_ptr = weights.data();
    const bool* matches_ptr = dict_matches.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = 0;
        // Use pointer arithmetic/indexing for tight loop performance
        for (std::size_t i = 0; i < n; ++i) {
            // Check if the dictionary entry pointed to by the code matches the prefix
            if (matches_ptr[codes_ptr[i]]) {
                current_iter_sum += weights_ptr[i];
            }
        }
        total_sum = current_iter_sum;
    }

    return total_sum;
}