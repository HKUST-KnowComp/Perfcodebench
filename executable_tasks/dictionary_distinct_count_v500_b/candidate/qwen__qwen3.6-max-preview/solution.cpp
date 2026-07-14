#include "interface.h"
#include <algorithm>
#include <vector>
#include <string>
#include <cstdint>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    (void)iters;
    if (dictionary.empty() || codes.empty()) return 0;

    std::vector<uint8_t> seen_idx(dictionary.size(), 0);
    std::vector<const std::string*> ptrs;
    ptrs.reserve(std::min(dictionary.size(), codes.size()));
    
    for (uint16_t c : codes) {
        if (c < dictionary.size() && !seen_idx[c]) {
            seen_idx[c] = 1;
            ptrs.push_back(&dictionary[c]);
        }
    }
    
    std::sort(ptrs.begin(), ptrs.end(), [](const std::string* a, const std::string* b) {
        return *a < *b;
    });
    
    auto last = std::unique(ptrs.begin(), ptrs.end(), [](const std::string* a, const std::string* b) {
        return *a == *b;
    });
    
    return static_cast<uint64_t>(last - ptrs.begin());
}
