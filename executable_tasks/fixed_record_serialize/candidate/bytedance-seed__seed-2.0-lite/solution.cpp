#include "interface.h"
#include <vector>
#include <cstdint>

namespace {
constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline void update_hash(uint64_t& hash, uint8_t b) {
    hash ^= static_cast<uint64_t>(b);
    hash *= FNV_PRIME;
}

inline void process_u32(uint64_t& hash, uint32_t v) {
    update_hash(hash, static_cast<uint8_t>(v));
    update_hash(hash, static_cast<uint8_t>(v >> 8));
    update_hash(hash, static_cast<uint8_t>(v >> 16));
    update_hash(hash, static_cast<uint8_t>(v >> 24));
}

inline void process_u16(uint64_t& hash, uint16_t v) {
    update_hash(hash, static_cast<uint8_t>(v));
    update_hash(hash, static_cast<uint8_t>(v >> 8));
}
} // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    const std::size_t n = ids.size();
    uint64_t final_hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = FNV_OFFSET;
        for (std::size_t i = 0; i < n; ++i) {
            process_u32(hash, ids[i]);
            process_u32(hash, ts[i]);
            process_u16(hash, levels[i]);
            process_u16(hash, lens[i]);
        }
        final_hash = hash;
    }
    return final_hash;
}