#include "interface.h"
#include <vector>
#include <algorithm>

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
    const uint32_t* k_ptr = keys.data();
    const uint32_t* v_ptr = values.data();
    const size_t k_size = keys.size();
    
    std::vector<uint32_t> remapped(probes.size());
    for (size_t i = 0; i < probes.size(); ++i) {
        const uint32_t* it = std::lower_bound(k_ptr, k_ptr + k_size, probes[i]);
        remapped[i] = v_ptr[it - k_ptr];
    }

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (uint32_t val : remapped) {
            hash ^= val;
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}
