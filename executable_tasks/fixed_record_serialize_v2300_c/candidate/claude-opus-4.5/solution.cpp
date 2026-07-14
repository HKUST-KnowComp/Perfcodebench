#include "interface.h"

#include <cstring>
#include <vector>

namespace {

inline uint64_t checksum_bytes_fast(const uint8_t* __restrict__ data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    constexpr uint64_t prime = 1099511628211ULL;
    
    size_t i = 0;
    size_t unroll_end = len & ~size_t(7);
    
    for (; i < unroll_end; i += 8) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(data[i + 1]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(data[i + 2]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(data[i + 3]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(data[i + 4]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(data[i + 5]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(data[i + 6]);
        hash *= prime;
        hash ^= static_cast<uint64_t>(data[i + 7]);
        hash *= prime;
    }
    
    for (; i < len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= prime;
    }
    
    return hash;
}

}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    
    const size_t n = ids.size();
    const size_t record_size = 12;  // 4 + 4 + 2 + 2 bytes per record
    const size_t total_size = n * record_size;
    
    std::vector<uint8_t> out(total_size);
    uint8_t* __restrict__ ptr = out.data();
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* __restrict__ p = ptr;
        
        for (size_t i = 0; i < n; ++i) {
            uint32_t id = ids[i];
            uint32_t t = ts[i];
            uint16_t level = levels[i];
            uint16_t len = lens[i];
            
            std::memcpy(p, &id, 4);
            std::memcpy(p + 4, &t, 4);
            std::memcpy(p + 8, &level, 2);
            std::memcpy(p + 10, &len, 2);
            p += 12;
        }
        
        hash = checksum_bytes_fast(ptr, total_size);
    }
    
    return hash;
}