#include "interface.h"
#include <vector>
#include <cstdint>

namespace {
inline uint64_t fnv1a_step(uint64_t hash, uint8_t byte) {
    return (hash ^ static_cast<uint64_t>(byte)) * 1099511628211ULL;
}

inline uint64_t update_hash(uint64_t hash, uint32_t v) {
    hash = fnv1a_step(hash, static_cast<uint8_t>(v));
    hash = fnv1a_step(hash, static_cast<uint8_t>(v >> 8));
    hash = fnv1a_step(hash, static_cast<uint8_t>(v >> 16));
    hash = fnv1a_step(hash, static_cast<uint8_t>(v >> 24));
    return hash;
}

inline uint64_t update_hash_u16(uint64_t hash, uint16_t v) {
    hash = fnv1a_step(hash, static_cast<uint8_t>(v));
    hash = fnv1a_step(hash, static_cast<uint8_t>(v >> 8));
    return hash;
}
}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    uint64_t final_hash = 0;
    const size_t n = ids.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = 1469598103934665603ULL;
        for (size_t i = 0; i < n; ++i) {
            hash = update_hash(hash, ids[i]);
            hash = update_hash(hash, ts[i]);
            hash = update_hash_u16(hash, levels[i]);
            hash = update_hash_u16(hash, lens[i]);
        }
        final_hash = hash;
    }
    return final_hash;
}