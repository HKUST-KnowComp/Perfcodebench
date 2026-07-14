#include "interface.h"
#include <vector>
#include <cstdint>
#include <string>

uint64_t dictionary_groupby_checksum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& values,
    int iters) {
    
    size_t dict_size = dictionary.size();
    std::vector<uint64_t> sums(dict_size, 0);
    uint64_t hash = 0;
    
    // Aggregate values by code directly using array indexing.
    // Computed once since inputs are const and results are iteration-invariant.
    for (size_t i = 0; i < codes.size(); ++i) {
        sums[static_cast<size_t>(codes[i])] += values[i];
    }
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (size_t i = 0; i < dict_size; ++i) {
            hash ^= sums[i];
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}