#include "interface.h"
#include <vector>
#include <cstddef>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    
    uint64_t hash = 0;
    const size_t n = ids.size();
    const uint32_t* id_ptr = ids.data();
    const uint32_t* ts_ptr = ts.data();
    const uint16_t* lvl_ptr = levels.data();
    const uint16_t* len_ptr = lens.data();
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (size_t i = 0; i < n; ++i) {
            uint32_t v = id_ptr[i];
            hash ^= static_cast<uint8_t>(v); hash *= FNV_PRIME;
            hash ^= static_cast<uint8_t>(v >> 8); hash *= FNV_PRIME;
            hash ^= static_cast<uint8_t>(v >> 16); hash *= FNV_PRIME;
            hash ^= static_cast<uint8_t>(v >> 24); hash *= FNV_PRIME;
            
            v = ts_ptr[i];
            hash ^= static_cast<uint8_t>(v); hash *= FNV_PRIME;
            hash ^= static_cast<uint8_t>(v >> 8); hash *= FNV_PRIME;
            hash ^= static_cast<uint8_t>(v >> 16); hash *= FNV_PRIME;
            hash ^= static_cast<uint8_t>(v >> 24); hash *= FNV_PRIME;
            
            uint16_t s = lvl_ptr[i];
            hash ^= static_cast<uint8_t>(s); hash *= FNV_PRIME;
            hash ^= static_cast<uint8_t>(s >> 8); hash *= FNV_PRIME;
            
            s = len_ptr[i];
            hash ^= static_cast<uint8_t>(s); hash *= FNV_PRIME;
            hash ^= static_cast<uint8_t>(s >> 8); hash *= FNV_PRIME;
        }
    }
    return hash;
}