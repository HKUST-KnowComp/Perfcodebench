#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

// FNV-1a constants
constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline uint64_t update_hash_u32(uint64_t hash, uint32_t v) {
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

inline uint64_t update_hash_u16(uint64_t hash, uint16_t v) {
    hash ^= static_cast<uint64_t>(v & 0xFF);
    hash *= FNV_PRIME;
    hash ^= static_cast<uint64_t>((v >> 8) & 0xFF);
    hash *= FNV_PRIME;
    return hash;
}

} // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    
    uint64_t final_hash = 0;
    size_t n = ids.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = FNV_OFFSET_BASIS;
        for (size_t i = 0; i < n; ++i) {
            hash = update_hash_u32(hash, ids[i]);
            hash = update_hash_u32(hash, ts[i]);
            hash = update_hash_u16(hash, levels[i]);
            hash = update_hash_u16(hash, lens[i]);
        }
        final_hash = hash;
    }
    
    return final_hash;
}