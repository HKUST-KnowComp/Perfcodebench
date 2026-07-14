#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline bool matches_prefix(const std::string& s) noexcept {
    return s[0] == 'q' && s[1] <= 'm';
}

} // namespace

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters)
{
    // Pre-compute for every dictionary index whether it passes the prefix filter.
    std::vector<bool> dict_ok(dictionary.size());
    for (size_t d = 0; d < dictionary.size(); ++d) {
        dict_ok[d] = matches_prefix(dictionary[d]);
    }

    uint64_t sum = 0;
    const size_t n = codes.size();

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t i = 0; i < n; ++i) {
            // Branch-predictable: most codes map to the same few strings.
            if (dict_ok[static_cast<size_t>(codes[i])]) {
                sum += static_cast<uint64_t>(weights[i]);
            }
        }
    }
    return sum;
}