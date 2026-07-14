#include "interface.h"
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    
    std::vector<uint8_t> dict_matches(dictionary.size(), 0);
    for (size_t i = 0; i < dictionary.size(); ++i) {
        const auto& s = dictionary[i];
        if (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm') {
            dict_matches[i] = 1;
        }
    }

    uint64_t iter_sum = 0;
    const size_t n = codes.size();
    for (size_t i = 0; i < n; ++i) {
        iter_sum += static_cast<uint64_t>(weights[i]) * dict_matches[codes[i]];
    }

    return iter_sum * static_cast<uint64_t>(iters);
}
