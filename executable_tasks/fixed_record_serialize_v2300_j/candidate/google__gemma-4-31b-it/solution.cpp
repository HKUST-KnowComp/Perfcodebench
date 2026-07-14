#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

// FNV-1a constants
constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

// Inline helper to update hash with a single byte
inline void update_hash_byte(uint64_t& hash, uint8_t b) {
    hash ^= static_cast<uint64_t>(b);
    hash *= FNV_PRIME;
}

// Inline helper to update hash with a 32-bit integer (little-endian)
inline void update_hash_u32(uint64_t& hash, uint32_t v) {
    update_hash_byte(hash, static_cast<uint8_t>(v));
    update_hash_byte(hash, static_cast<uint8_t>(v >> 8));
    update_hash_byte(hash, static_cast<uint8_t>(v >> 16));
    update_hash_byte(hash, static_cast<uint8_t>(v >> 24));
}

// Inline helper to update hash with a 16-bit integer (little-endian)
inline void update_hash_u16(uint64_t& hash, uint16_t v) {
    update_hash_byte(hash, static_cast<uint8_t>(v));
    update_hash_byte(hash, static_cast<uint8_t>(v >> 8));
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
            update_hash_u32(hash, ids[i]);
            update_hash_u32(hash, ts[i]);
            update_hash_u16(hash, levels[i]);
            update_hash_u16(hash, lens[i]);
        }
        final_hash = hash;
    }
    
    return final_hash;
}