#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

// FNV-1a constants
constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

// Inline helper to update hash with a value treated as little-endian bytes
template <typename T>
inline void update_hash_le(uint64_t& hash, T value) {
    for (size_t i = 0; i < sizeof(T); ++i) {
        hash ^= static_cast<uint64_t>(static_cast<uint8_t>(value >> (i * 8)));
        hash *= FNV_PRIME;
    }
}

} // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    
    uint64_t final_hash = 0;
    const size_t n = ids.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = FNV_OFFSET_BASIS;
        
        for (size_t i = 0; i < n; ++i) {
            // Serialize ids[i] (u32)
            update_hash_le(hash, ids[i]);
            // Serialize ts[i] (u32)
            update_hash_le(hash, ts[i]);
            // Serialize levels[i] (u16)
            update_hash_le(hash, levels[i]);
            // Serialize lens[i] (u16)
            update_hash_le(hash, lens[i]);
        }
        final_hash = hash;
    }
    
    return final_hash;
}