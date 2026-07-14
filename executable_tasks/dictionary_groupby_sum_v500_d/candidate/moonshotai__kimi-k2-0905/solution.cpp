#include "interface.h"
#include "stdlib.h"
#include <cstring>
#include <vector>

static inline uint64_t mix(uint64_t hash, uint64_t value) {
    register uint64_t res = hash ^ value;
    res *= UINT64_C(1099511628211);
    return res;
}

uint64_t dictionary_groupby_checksum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& values,
    int iters) {
    const size_t dict_size = dictionary.size();
    const size_t n = codes.size();
    std::vector<uint64_t> dict_sums(dict_size, 0);
    uint64_t hash = UINT64_C(14695981039346656037);
    
    for (int iter = 0; iter < iters; ++iter) {
        // reset accumulators - memset is fastest
        std::memset(dict_sums.data(), 0, dict_size * sizeof(uint64_t));
        
        // compute per-code sums in a tight sequential pass
        for (size_t i = 0; i < n; ++i) {
            uint64_t* ptr = dict_sums.data() + codes[i];
            *ptr += static_cast<uint64_t>(values[i]);
        }
        
        // fold into hash
        const uint64_t* p = dict_sums.data();
        for (size_t i = 0; i < dict_size; ++i) {
            hash = mix(hash, p[i]);
        }
    }
    return hash;
}