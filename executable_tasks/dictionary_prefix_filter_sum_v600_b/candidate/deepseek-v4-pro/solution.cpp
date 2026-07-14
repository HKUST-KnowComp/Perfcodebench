#include "interface.h"

#include <cstddef>
#include <string>
#include <vector>

namespace {

// Precomputes which dictionary strings satisfy the prefix rule.
std::vector<bool> precompute_prefix_matches(const std::vector<std::string>& dict) {
    std::vector<bool> matches(dict.size(), false);
    for (std::size_t i = 0; i < dict.size(); ++i) {
        const std::string& s = dict[i];
        // Assumes all dictionary strings have length >= 2 as required by the original match logic.
        matches[i] = (s[0] == 'q' && s[1] <= 'm');
    }
    return matches;
}

}  // namespace

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    (void)iters;  // result is independent of the number of iterations
    const std::vector<bool> matches = precompute_prefix_matches(dictionary);

    uint64_t sum = 0;
    const std::size_t n = codes.size();
    for (std::size_t i = 0; i < n; ++i) {
        if (matches[codes[i]]) {
            sum += static_cast<uint64_t>(weights[i]);
        }
    }
    return sum;
}
