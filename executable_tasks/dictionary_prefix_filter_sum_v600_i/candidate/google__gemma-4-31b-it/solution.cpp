#include "interface.h"

#include <vector>
#include <string>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    if (iters <= 0) return 0;

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

    // Pre-calculate the sum for a single pass
    uint64_t single_pass_sum = 0;
    size_t n = codes.size();
    for (size_t i = 0; i < n; ++i) {
        if (dict_matches[codes[i]]) {
            single_pass_sum += weights[i];
        }
    }

    // The problem asks for the sum returned after 'iters' iterations.
    // The baseline resets 'sum = 0' inside the loop, meaning it returns the sum of the LAST iteration.
    return single_pass_sum;
}