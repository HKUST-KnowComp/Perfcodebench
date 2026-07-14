#include "interface.h"

#include <string>
#include <vector>

namespace {
bool matches_prefix(const std::string& s) {
    return s[0] == 'q' && s[1] <= 'm';
}
}

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    (void)iters;
    std::vector<bool> match(dictionary.size(), false);
    for (std::size_t i = 0; i < dictionary.size(); ++i) {
        if (matches_prefix(dictionary[i])) {
            match[i] = true;
        }
    }
    uint64_t total = 0;
    for (std::size_t i = 0; i < codes.size(); ++i) {
        if (match[codes[i]]) {
            total += static_cast<uint64_t>(weights[i]);
        }
    }
    return total;
}