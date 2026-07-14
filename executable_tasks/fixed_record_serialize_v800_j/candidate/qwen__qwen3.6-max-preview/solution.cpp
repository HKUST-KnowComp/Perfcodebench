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
    
    std::size_t n = ids.size();
    std::size_t buf_size = n * 12;
    std::vector<uint8_t> buf(buf_size);
    uint64_t hash = 0;
    
    const uint32_t* ids_ptr = ids.data();
    const uint32_t* ts_ptr = ts.data();
    const uint16_t* levels_ptr = levels.data();
    const uint16_t* lens_ptr = lens.data();
    
    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* p = buf.data();
        for (std::size_t i = 0; i < n; ++i) {
            uint32_t id = ids_ptr[i];
            uint32_t t = ts_ptr[i];
            uint16_t lv = levels_ptr[i];
            uint16_t ln = lens_ptr[i];
            
            p[0] = id; p[1] = id >> 8; p[2] = id >> 16; p[3] = id >> 24;
            p[4] = t;  p[5] = t >> 8;  p[6] = t >> 16;  p[7] = t >> 24;
            p[8] = lv; p[9] = lv >> 8;
            p[10] = ln; p[11] = ln >> 8;
            p += 12;
        }
        
        hash = 1469598103934665603ULL;
        const uint8_t* ptr = buf.data();
        const uint8_t* end = ptr + buf_size;
        while (ptr < end) {
            hash ^= *ptr++;
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}
