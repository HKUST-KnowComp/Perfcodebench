#include "interface.h"
#include <vector>
#include <algorithm>

uint64_t dictionary_groupby_checksum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& values,
    int iters) {
    const size_t dict_size = dictionary.size();
    std::vector<uint64_t> sums(dict_size, 0);
    uint64_t hash = 0;

    const size_t n = codes.size();
    const uint16_t* c = codes.data();
    const uint32_t* v = values.data();

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(sums.begin(), sums.end(), 0);
        for (size_t i = 0; i < n; ++i) {
            sums[c[i]] += static_cast<uint64_t>(v[i]);
        }
        uint64_t h = 1469598103934665603ULL;
        for (size_t i = 0; i < dict_size; ++i) {
            h ^= sums[i];
            h *= 1099511628211ULL;
        }
        hash = h;
    }
    return hash;
}