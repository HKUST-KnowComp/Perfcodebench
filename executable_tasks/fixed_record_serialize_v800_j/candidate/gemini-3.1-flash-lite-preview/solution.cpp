#include "interface.h"
#include <vector>
#include <cstring>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    
    size_t n = ids.size();
    // Each record is 4+4+2+2 = 12 bytes
    size_t record_size = 12;
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        
        for (size_t i = 0; i < n; ++i) {
            uint32_t id = ids[i];
            uint32_t t = ts[i];
            uint16_t lvl = levels[i];
            uint16_t len = lens[i];
            
            uint8_t buf[12];
            std::memcpy(buf, &id, 4);
            std::memcpy(buf + 4, &t, 4);
            std::memcpy(buf + 8, &lvl, 2);
            std::memcpy(buf + 10, &len, 2);
            
            for (int j = 0; j < 12; ++j) {
                hash ^= static_cast<uint64_t>(buf[j]);
                hash *= 1099511628211ULL;
            }
        }
    }
    
    return hash;
}