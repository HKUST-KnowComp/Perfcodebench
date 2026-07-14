#include "interface.h"
#include <vector>
#include <cstring>

uint64_t dictionary_groupby_checksum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& values,
    int iters) {
    size_t dict_size = dictionary.size();
    std::vector<uint64_t> sums(dict_size);
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        std::memset(sums.data(), 0, dict_size * sizeof(uint64_t));
        size_t n = codes.size();
        for (size_t i = 0; i < n; ++i) {
            sums[codes[i]] += values[i];
        }
        hash = 1469598103934665603ULL;
        for (size_t j = 0; j < dict_size; ++j) {
            hash ^= sums[j];
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}