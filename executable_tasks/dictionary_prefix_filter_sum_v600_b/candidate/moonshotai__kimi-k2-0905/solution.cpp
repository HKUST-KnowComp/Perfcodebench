#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters)
{
    const std::size_t dict_size = dictionary.size();
    // 1-bit per dictionary entry: 1 if the string matches prefix rule
    std::vector<uint8_t> match_mask(dict_size, 0);
    for (std::size_t i = 0; i < dict_size; ++i) {
        const std::string& s = dictionary[i];
        // prefix rule: s[0]=='q' && s[1]<='m'
        match_mask[i] = (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm') ? 1 : 0;
    }

    const std::size_t n = codes.size();
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local = 0;
        for (std::size_t i = 0; i < n; ++i) {
            const uint16_t code = codes[i];
            local += match_mask[code] * static_cast<uint64_t>(weights[i]);
        }
        sum = local;
    }
    return sum;
}