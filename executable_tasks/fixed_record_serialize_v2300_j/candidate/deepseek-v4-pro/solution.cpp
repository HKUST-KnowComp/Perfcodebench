#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline void update_hash_u32(uint64_t& hash, uint32_t v) {
    // little-endian byte order as in original write_u32
    hash ^= static_cast<uint8_t>(v);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint8_t>(v >> 8);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint8_t>(v >> 16);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint8_t>(v >> 24);
    hash *= 1099511628211ULL;
}

inline void update_hash_u16(uint64_t& hash, uint16_t v) {
    hash ^= static_cast<uint8_t>(v);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint8_t>(v >> 8);
    hash *= 1099511628211ULL;
}

} // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {

    if (iters <= 0) {
        return 0;
    }

    const size_t n = ids.size();
    uint64_t hash = 1469598103934665603ULL; // initial hash value

    const uint32_t* pIds = ids.data();
    const uint32_t* pTs = ts.data();
    const uint16_t* pLevels = levels.data();
    const uint16_t* pLens = lens.data();

    for (size_t i = 0; i < n; ++i) {
        update_hash_u32(hash, pIds[i]);
        update_hash_u32(hash, pTs[i]);
        update_hash_u16(hash, pLevels[i]);
        update_hash_u16(hash, pLens[i]);
    }

    return hash;
}
