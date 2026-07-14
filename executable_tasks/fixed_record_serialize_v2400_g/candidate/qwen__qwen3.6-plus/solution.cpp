#include "interface.h"
#include <cstddef>
#include <cstdint>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    
    const size_t n = ids.size();
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    
    const uint32_t* p_ids = ids.data();
    const uint32_t* p_ts = ts.data();
    const uint16_t* p_levels = levels.data();
    const uint16_t* p_lens = lens.data();
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = FNV_OFFSET;
        for (size_t i = 0; i < n; ++i) {
            uint32_t id = p_ids[i];
            h ^= (uint8_t)id; h *= FNV_PRIME;
            h ^= (uint8_t)(id >> 8); h *= FNV_PRIME;
            h ^= (uint8_t)(id >> 16); h *= FNV_PRIME;
            h ^= (uint8_t)(id >> 24); h *= FNV_PRIME;
            
            uint32_t t = p_ts[i];
            h ^= (uint8_t)t; h *= FNV_PRIME;
            h ^= (uint8_t)(t >> 8); h *= FNV_PRIME;
            h ^= (uint8_t)(t >> 16); h *= FNV_PRIME;
            h ^= (uint8_t)(t >> 24); h *= FNV_PRIME;
            
            uint16_t l = p_levels[i];
            h ^= (uint8_t)l; h *= FNV_PRIME;
            h ^= (uint8_t)(l >> 8); h *= FNV_PRIME;
            
            uint16_t le = p_lens[i];
            h ^= (uint8_t)le; h *= FNV_PRIME;
            h ^= (uint8_t)(le >> 8); h *= FNV_PRIME;
        }
        hash = h;
    }
    return hash;
}