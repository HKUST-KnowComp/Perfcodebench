#include "interface.h"

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    const size_t num_records = ids.size();
    constexpr uint64_t kFnvOffset = 1469598103934665603ULL;
    constexpr uint64_t kFnvPrime = 1099511628211ULL;
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = kFnvOffset;
        for (size_t i = 0; i < num_records; ++i) {
            // Serialize id (u32 little-endian, matches original write order)
            uint32_t id = ids[i];
            hash ^= id & 0xFF; hash *= kFnvPrime;
            hash ^= (id >> 8) & 0xFF; hash *= kFnvPrime;
            hash ^= (id >> 16) & 0xFF; hash *= kFnvPrime;
            hash ^= (id >> 24) & 0xFF; hash *= kFnvPrime;

            // Serialize timestamp (u32)
            uint32_t t = ts[i];
            hash ^= t & 0xFF; hash *= kFnvPrime;
            hash ^= (t >> 8) & 0xFF; hash *= kFnvPrime;
            hash ^= (t >> 16) & 0xFF; hash *= kFnvPrime;
            hash ^= (t >> 24) & 0xFF; hash *= kFnvPrime;

            // Serialize level (u16)
            uint16_t lvl = levels[i];
            hash ^= lvl & 0xFF; hash *= kFnvPrime;
            hash ^= (lvl >> 8) & 0xFF; hash *= kFnvPrime;

            // Serialize length (u16)
            uint16_t len = lens[i];
            hash ^= len & 0xFF; hash *= kFnvPrime;
            hash ^= (len >> 8) & 0xFF; hash *= kFnvPrime;
        }
        final_hash = hash;
    }
    return final_hash;
}