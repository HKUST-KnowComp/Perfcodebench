#include "interface.h"
#include <string>
#include <vector>

namespace {

// The prefix rule: s[0] == 'q' and s[1] <= 'm'
// We check length to prevent out-of-bounds access if strings are short.
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

    // Pre-calculate which dictionary entries satisfy the prefix condition.
    // This avoids repeated string access and comparison inside the 'iters' loop.
    std::vector<bool> dict_matches(dictionary.size());
    for (std::size_t i = 0; i < dictionary.size(); ++i) {
        dict_matches[i] = matches_prefix(dictionary[i]);
    }

    // Pre-calculate the match status for each code in the sequence.
    // This transforms the problem into a simple sum of weights where the code matches.
    std::vector<bool> code_matches(codes.size());
    for (std::size_t i = 0; i < codes.size(); ++i) {
        code_matches[i] = dict_matches[static_cast<std::size_t>(codes[i])];
    }

    uint64_t total_sum = 0;
    // The loop now only performs a simple conditional addition.
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = 0;
        for (std::size_t i = 0; i < codes.size(); ++i) {
            if (code_matches[i]) {
                current_iter_sum += static_cast<uint64_t>(weights[i]);
            }
        }
        total_sum = current_iter_sum;
    }

    return total_sum;
}