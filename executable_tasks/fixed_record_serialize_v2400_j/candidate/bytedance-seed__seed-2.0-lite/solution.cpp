#include "interface.h"

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
    constexpr uint64_t FNV_PRIME = 1099511628211ULL;
    const size_t n = ids.size();
    
    const uint32_t* const ids_ptr = ids.data();
    const uint32_t* const ts_ptr = ts.data();
    const uint16_t* const levels_ptr = levels.data();
    const uint16_t* const lens_ptr = lens.data();

    uint64_t final_hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = FNV_OFFSET;
        for (size_t i = 0; i < n; ++i) {
            // Process ID (little-endian byte order matching baseline)
            uint32_t id = ids_ptr[i];
            hash ^= static_cast<uint64_t>(id & 0xFF);
            hash *= FNV_PRIME;
            hash ^= static_cast<uint64_t>((id >> 8) & 0xFF);
            hash *= FNV_PRIME;
            hash ^= static_cast<uint64_t>((id >> 16) & 0xFF);
            hash *= FNV_PRIME;
            hash ^= static_cast<uint64_t>((id >> 24) & 0xFF);
            hash *= FNV_PRIME;

            // Process timestamp
            uint32_t t = ts_ptr[i];
            hash ^= static_cast<uint64_t>(t & 0xFF);
            hash *= FNV_PRIME;
            hash ^= static_cast<uint64_t>((t >> 8) & 0xFF);
            hash *= FNV_PRIME;
            hash ^= static_cast<uint64_t>((t >> 16) & 0xFF);
            hash *= FNV_PRIME;
            hash ^= static_cast<uint64_t>((t >> 24) & 0xFF);
            hash *= FNV_PRIME;

            // Process level
            uint16_t lvl = levels_ptr[i];
            hash ^= static_cast<uint64_t>(lvl & 0xFF);
            hash *= FNV_PRIME;
            hash ^= static_cast<uint64_t>((lvl >> 8) & 0xFF);
            hash *= FNV_PRIME;

            // Process length
            uint16_t len = lens_ptr[i];
            hash ^= static_cast<uint64_t>(len & 0xFF);
            hash *= FNV_PRIME;
            hash ^= static_cast<uint64_t>((len >> 8) & 0xFF);
            hash *= FNV_PRIME;
        }
        final_hash = hash;
    }
    return final_hash;
}