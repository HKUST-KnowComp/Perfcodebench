#include "interface.h"
#include <vector>
#include <cstdint>
#include <algorithm>

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
    
    if (iters <= 0) return 0;

    uint32_t max_key = 0;
    for (uint32_t k : keys) {
        if (k > max_key) max_key = k;
    }

    std::vector<uint32_t> table(max_key + 1);
    for (size_t i = 0; i < keys.size(); ++i) {
        table[keys[i]] = values[i];
    }

    const uint32_t* t_ptr = table.data();
    const uint32_t* p_ptr = probes.data();
    const size_t p_size = probes.size();
    
    uint64_t hash = 0;
    constexpr uint64_t FNV_PRIME = 1099511628211ULL;
    constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = FNV_OFFSET;
        for (size_t i = 0; i < p_size; ++i) {
            h ^= t_ptr[p_ptr[i]];
            h *= FNV_PRIME;
        }
        hash = h;
    }
    return hash;
}