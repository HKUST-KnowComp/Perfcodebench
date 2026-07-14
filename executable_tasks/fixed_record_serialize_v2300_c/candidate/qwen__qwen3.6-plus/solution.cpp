#include "interface.h"
#include <vector>
#include <cstddef>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    
    const size_t n = ids.size();
    if (n == 0) {
        uint64_t h = 1469598103934665603ULL;
        for (int i = 0; i < iters; ++i) h = 1469598103934665603ULL;
        return h;
    }

    const size_t total_bytes = n * 12;
    std::vector<uint8_t> out(total_bytes);
    uint64_t hash = 0;
    
    const uint32_t* id_ptr = ids.data();
    const uint32_t* ts_ptr = ts.data();
    const uint16_t* lv_ptr = levels.data();
    const uint16_t* ln_ptr = lens.data();
    
    const uint64_t FNV_PRIME = 1099511628211ULL;
    
    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* p = out.data();
        hash = 1469598103934665603ULL;
        
        for (size_t i = 0; i < n; ++i) {
            uint32_t id = id_ptr[i];
            uint8_t b;
            
            b = static_cast<uint8_t>(id); hash ^= b; hash *= FNV_PRIME; *p++ = b;
            b = static_cast<uint8_t>(id >> 8); hash ^= b; hash *= FNV_PRIME; *p++ = b;
            b = static_cast<uint8_t>(id >> 16); hash ^= b; hash *= FNV_PRIME; *p++ = b;
            b = static_cast<uint8_t>(id >> 24); hash ^= b; hash *= FNV_PRIME; *p++ = b;
            
            uint32_t t = ts_ptr[i];
            b = static_cast<uint8_t>(t); hash ^= b; hash *= FNV_PRIME; *p++ = b;
            b = static_cast<uint8_t>(t >> 8); hash ^= b; hash *= FNV_PRIME; *p++ = b;
            b = static_cast<uint8_t>(t >> 16); hash ^= b; hash *= FNV_PRIME; *p++ = b;
            b = static_cast<uint8_t>(t >> 24); hash ^= b; hash *= FNV_PRIME; *p++ = b;
            
            uint16_t l = lv_ptr[i];
            b = static_cast<uint8_t>(l); hash ^= b; hash *= FNV_PRIME; *p++ = b;
            b = static_cast<uint8_t>(l >> 8); hash ^= b; hash *= FNV_PRIME; *p++ = b;
            
            uint16_t le = ln_ptr[i];
            b = static_cast<uint8_t>(le); hash ^= b; hash *= FNV_PRIME; *p++ = b;
            b = static_cast<uint8_t>(le >> 8); hash ^= b; hash *= FNV_PRIME; *p++ = b;
        }
    }
    return hash;
}