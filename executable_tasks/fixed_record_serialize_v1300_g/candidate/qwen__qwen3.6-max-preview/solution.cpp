#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstddef>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    
    const size_t n = ids.size();
    std::vector<uint8_t> out(n * 12);
    uint64_t hash = 0;
    
    const uint32_t* p_ids = ids.data();
    const uint32_t* p_ts = ts.data();
    const uint16_t* p_lv = levels.data();
    const uint16_t* p_ln = lens.data();
    
    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* ptr = out.data();
        for (size_t i = 0; i < n; ++i) {
            uint32_t id = p_ids[i];
            ptr[0] = static_cast<uint8_t>(id);
            ptr[1] = static_cast<uint8_t>(id >> 8);
            ptr[2] = static_cast<uint8_t>(id >> 16);
            ptr[3] = static_cast<uint8_t>(id >> 24);
            
            uint32_t t = p_ts[i];
            ptr[4] = static_cast<uint8_t>(t);
            ptr[5] = static_cast<uint8_t>(t >> 8);
            ptr[6] = static_cast<uint8_t>(t >> 16);
            ptr[7] = static_cast<uint8_t>(t >> 24);
            
            uint16_t lv = p_lv[i];
            ptr[8] = static_cast<uint8_t>(lv);
            ptr[9] = static_cast<uint8_t>(lv >> 8);
            
            uint16_t ln = p_ln[i];
            ptr[10] = static_cast<uint8_t>(ln);
            ptr[11] = static_cast<uint8_t>(ln >> 8);
            
            ptr += 12;
        }
        
        hash = 1469598103934665603ULL;
        const uint8_t* data = out.data();
        const uint8_t* end = data + out.size();
        while (data < end) {
            hash ^= *data++;
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}