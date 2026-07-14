#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstring>

namespace {

inline bool matches_prefix(const std::string& s) {
    return s.size() >= 2 && s[0] == 'q' && s[1] <= 'm';
}

} // namespace

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters)
{
    const size_t n_codes = codes.size();
    const size_t n_dict  = dictionary.size();

    // Pre-compute match flags for every dictionary entry (1 = matches, 0 = doesn't)
    std::vector<uint8_t> dict_match(n_dict, 0);
    for (size_t d = 0; d < n_dict; ++d) {
        dict_match[d] = matches_prefix(dictionary[d]) ? 1 : 0;
    }

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t i = 0; i < n_codes; ++i) {
            const uint16_t code = codes[i];
            // The problem guarantees codes are in range; keep the check in debug builds only
            sum += dict_match[code] * static_cast<uint64_t>(weights[i]);
        }
    }
    return sum;
}