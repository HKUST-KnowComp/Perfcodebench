#include "interface.h"

#include <cstdint>
#include <cstring>
#include <vector>

uint64_t dictionary_groupby_checksum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& values,
    int iters) {
    
    size_t dict_size = dictionary.size();
    std::vector<uint64_t> sums(dict_size, 0);
    uint64_t hash = 0;
    size_t n = codes.size();
    
    const uint16_t* codes_ptr = codes.data();
    const uint32_t* values_ptr = values.data();
    uint64_t* sums_ptr = sums.data();

    for (int iter = 0; iter < iters; ++iter) {
        std::memset(sums_ptr, 0, dict_size * sizeof(uint64_t));
        
        for (size_t i = 0; i < n; ++i) {
            sums_ptr[codes_ptr[i]] += values_ptr[i];
        }
        
        hash = 1469598103934665603ULL;
        for (size_t k = 0; k < dict_size; ++k) {
            hash ^= sums_ptr[k];
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}