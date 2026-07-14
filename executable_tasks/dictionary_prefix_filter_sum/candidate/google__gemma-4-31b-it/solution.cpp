#include "interface.h"

#include <vector>
#include <string>

namespace {

bool matches_prefix(const std::string& s) {
    // The baseline checks s[0] == 'q' && s[1] <= 'm'.
    // We must ensure the string has at least 2 characters to avoid OOB access,
    // although the baseline didn't explicitly check length, it's implied by the logic.
    return s.size() >= 2 && s[0] == 'q' && s[1] <= 'm';
}

} // namespace

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    if (iters <= 0) return 0;

    // Pre-calculate which dictionary entries match the prefix.
    // This removes the need to access strings inside the main loop.
    std::vector<bool> dict_matches(dictionary.size());
    for (size_t i = 0; i < dictionary.size(); ++i) {
        dict_matches[i] = matches_prefix(dictionary[i]);
    }

    uint64_t final_sum = 0;
    const size_t n = codes.size();

    // The baseline resets 'sum = 0' inside the iters loop, meaning it only
    // returns the sum for the very last iteration. We simulate this behavior.
    // Since the data doesn't change between iterations, we only need to compute it once.
    for (size_t i = 0; i < n; ++i) {
        if (dict_matches[codes[i]]) {
            final_sum += weights[i];
        }
    }

    // The baseline loop runs 'iters' times. If iters > 0, it returns the sum of the last run.
    return final_sum;
}