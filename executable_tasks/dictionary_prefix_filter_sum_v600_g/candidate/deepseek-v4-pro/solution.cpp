#include "interface.h"

#include <string>
#include <vector>

namespace {

bool matches_prefix(const std::string& s) {
    return s[0] == 'q' && s[1] <= 'm';
}

}  // namespace

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    if (iters <= 0) return 0;

    // Precompute prefix match flag for each dictionary entry
    const size_t dict_size = dictionary.size();
    std::vector<bool> match(dict_size, false);
    for (size_t i = 0; i < dict_size; ++i) {
        match[i] = matches_prefix(dictionary[i]);
    }

    uint64_t sum = 0;
    const size_t n = codes.size();
    for (size_t i = 0; i < n; ++i) {
        if (match[codes[i]]) {
            sum += static_cast<uint64_t>(weights[i]);
        }
    }
    return sum;
}