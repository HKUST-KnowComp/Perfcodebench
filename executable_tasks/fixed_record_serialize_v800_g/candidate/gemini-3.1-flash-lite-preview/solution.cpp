#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;

    auto update_hash = [&](uint64_t val, int bytes) {
        for (int i = 0; i < bytes; ++i) {
            hash ^= (val & 0xFF);
            hash *= FNV_PRIME;
            val >>= 8;
        }
    };

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        size_t n = ids.size();
        for (size_t i = 0; i < n; ++i) {
            update_hash(ids[i], 4);
            update_hash(ts[i], 4);
            update_hash(levels[i], 2);
            update_hash(lens[i], 2);
        }
    }
    return hash;
}