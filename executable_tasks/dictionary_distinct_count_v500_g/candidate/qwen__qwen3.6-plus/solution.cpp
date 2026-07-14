#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    (void)iters;
    if (codes.empty()) return 0;

    std::vector<uint8_t> seen(65536, 0);
    std::vector<uint16_t> unique_codes;
    unique_codes.reserve(codes.size() < 65536 ? codes.size() : 65536);

    for (uint16_t c : codes) {
        if (!seen[c]) {
            seen[c] = 1;
            unique_codes.push_back(c);
        }
    }

    if (unique_codes.empty()) return 0;

    std::sort(unique_codes.begin(), unique_codes.end(), [&](uint16_t a, uint16_t b) {
        return dictionary[a] < dictionary[b];
    });

    uint64_t count = 1;
    for (size_t i = 1; i < unique_codes.size(); ++i) {
        if (dictionary[unique_codes[i]] != dictionary[unique_codes[i-1]]) {
            ++count;
        }
    }
    return count;
}