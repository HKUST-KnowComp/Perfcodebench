#include "interface.h"

#include <string>
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    
    std::vector<uint8_t> matches(dictionary.size());
    for (size_t i = 0; i < dictionary.size(); ++i) {
        const auto& s = dictionary[i];
        matches[i] = (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm') ? 1 : 0;
    }

    uint64_t sum = 0;
    const uint16_t* codes_ptr = codes.data();
    const uint32_t* weights_ptr = weights.data();
    size_t n = codes.size();

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t i = 0; i < n; ++i) {
            if (matches[codes_ptr[i]]) {
                sum += weights_ptr[i];
            }
        }
    }
    return sum;
}