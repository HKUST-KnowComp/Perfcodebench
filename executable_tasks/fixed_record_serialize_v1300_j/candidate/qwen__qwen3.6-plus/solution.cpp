#include "interface.h"

#include <vector>
#include <cstddef>

namespace {

constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline void fnv1a_u32(uint64_t& h, uint32_t v) {
    h ^= static_cast<uint64_t>(v & 0xFF); h *= FNV_PRIME;
    h ^= static_cast<uint64_t>((v >> 8) & 0xFF); h *= FNV_PRIME;
    h ^= static_cast<uint64_t>((v >> 16) & 0xFF); h *= FNV_PRIME;
    h ^= static_cast<uint64_t>((v >> 24) & 0xFF); h *= FNV_PRIME;
}

inline void fnv1a_u16(uint64_t& h, uint16_t v) {
    h ^= static_cast<uint64_t>(v & 0xFF); h *= FNV_PRIME;
    h ^= static_cast<uint64_t>((v >> 8) & 0xFF); h *= FNV_PRIME;
}

} // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    
    const size_t n = ids.size();
    const uint32_t* p_ids = ids.data();
    const uint32_t* p_ts = ts.data();
    const uint16_t* p_levels = levels.data();
    const uint16_t* p_lens = lens.data();
    
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (size_t i = 0; i < n; ++i) {
            fnv1a_u32(hash, p_ids[i]);
            fnv1a_u32(hash, p_ts[i]);
            fnv1a_u16(hash, p_levels[i]);
            fnv1a_u16(hash, p_lens[i]);
        }
    }
    return hash;
}
