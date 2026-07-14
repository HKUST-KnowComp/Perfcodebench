#include "interface.h"
#include <vector>
#include <string>
#include <cstdint>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    const std::size_t dsize = dictionary.size();
    std::vector<uint8_t> match_mask(dsize);
    for (std::size_t i = 0; i < dsize; ++i) {
        match_mask[i] = (dictionary[i][0] == 'q' && dictionary[i][1] <= 'm') ? 1 : 0;
    }

    uint64_t sum = 0;
    const std::size_t n = codes.size();
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local_sum = 0;
        for (std::size_t i = 0; i < n; ++i) {
            if (match_mask[static_cast<std::size_t>(codes[i])]) {
                local_sum += static_cast<uint64_t>(weights[i]);
            }
        }
        sum = local_sum;
    }
    return sum;
}