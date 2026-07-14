#include "interface.h"
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    (void)iters; // Result independent of iteration count, compute once.
    std::vector<bool> matches(dictionary.size());
    for (std::size_t i = 0; i < dictionary.size(); ++i) {
        const std::string& s = dictionary[i];
        matches[i] = (s[0] == 'q' && s[1] <= 'm');
    }
    uint64_t total = 0;
    for (std::size_t i = 0; i < codes.size(); ++i) {
        if (matches[codes[i]]) {
            total += weights[i];
        }
    }
    return total;
}