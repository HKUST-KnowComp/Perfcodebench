#include "interface.h"

#include <cstring>
#include <vector>

namespace {

static constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
static constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline uint64_t fnv1a_u32(uint64_t hash, uint32_t v) {
    hash ^= static_cast<uint64_t>(v & 0xFF);
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>((v >> 8) & 0xFF);
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>((v >> 16) & 0xFF);
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>((v >> 24) & 0xFF);
    hash *= FNV_PRIME;
    return hash;
}

inline uint64_t fnv1a_u16(uint64_t hash, uint16_t v) {
    hash ^= static_cast<uint64_t>(v & 0xFF);
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>((v >> 8) & 0xFF);
    hash *= FNV_PRIME;
    return hash;
}

inline uint64_t fnv1a_record(uint64_t hash, uint32_t id, uint32_t t, uint16_t level, uint16_t len) {
    hash = fnv1a_u32(hash, id);
    hash = fnv1a_u32(hash, t);
    hash = fnv1a_u16(hash, level);
    hash = fnv1a_u16(hash, len);
    return hash;
}

}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    
    const std::size_t n = ids.size();
    uint64_t hash = 0;
    
    const uint32_t* __restrict__ id_ptr = ids.data();
    const uint32_t* __restrict__ ts_ptr = ts.data();
    const uint16_t* __restrict__ lv_ptr = levels.data();
    const uint16_t* __restrict__ ln_ptr = lens.data();
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        
        std::size_t i = 0;
        const std::size_t n4 = n & ~std::size_t(3);
        
        for (; i < n4; i += 4) {
            hash = fnv1a_record(hash, id_ptr[i], ts_ptr[i], lv_ptr[i], ln_ptr[i]);
            hash = fnv1a_record(hash, id_ptr[i+1], ts_ptr[i+1], lv_ptr[i+1], ln_ptr[i+1]);
            hash = fnv1a_record(hash, id_ptr[i+2], ts_ptr[i+2], lv_ptr[i+2], ln_ptr[i+2]);
            hash = fnv1a_record(hash, id_ptr[i+3], ts_ptr[i+3], lv_ptr[i+3], ln_ptr[i+3]);
        }
        
        for (; i < n; ++i) {
            hash = fnv1a_record(hash, id_ptr[i], ts_ptr[i], lv_ptr[i], ln_ptr[i]);
        }
    }
    
    return hash;
}