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
    size_t record_size = 12; // 4 + 4 + 2 + 2
    size_t total_size = n * record_size;
    std::vector<uint8_t> buffer(total_size);
    
    uint64_t final_hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* ptr = buffer.data();
        uint64_t hash = 1469598103934665603ULL;
        
        for (size_t i = 0; i < n; ++i) {
            uint32_t id = ids[i];
            uint32_t t = ts[i];
            uint16_t l1 = levels[i];
            uint16_t l2 = lens[i];
            
            std::memcpy(ptr, &id, 4);
            std::memcpy(ptr + 4, &t, 4);
            std::memcpy(ptr + 8, &l1, 2);
            std::memcpy(ptr + 10, &l2, 2);
            ptr += 12;
        }
        
        // FNV-1a checksum calculation
        const uint8_t* data = buffer.data();
        for (size_t i = 0; i < total_size; ++i) {
            hash ^= static_cast<uint64_t>(data[i]);
            hash *= 1099511628211ULL;
        }
        final_hash = hash;
    }
    
    return final_hash;
}