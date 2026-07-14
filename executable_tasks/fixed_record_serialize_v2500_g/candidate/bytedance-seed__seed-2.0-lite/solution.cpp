#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    const size_t num_records = ids.size();
    const uint64_t FNV_PRIME = 1099511628211ULL;
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = 1469598103934665603ULL;
        for (size_t i = 0; i < num_records; ++i) {
            // Serialize and hash ids[i] (little-endian byte order matching baseline)
            uint32_t v = ids[i];
            hash ^= static_cast<uint8_t>(v);
            hash *= FNV_PRIME;
            hash ^= static_cast<uint8_t>(v >> 8);
            hash *= FNV_PRIME;
            hash ^= static_cast<uint8_t>(v >> 16);
            hash *= FNV_PRIME;
            hash ^= static_cast<uint8_t>(v >> 24);
            hash *= FNV_PRIME;

            // Serialize and hash ts[i]
            v = ts[i];
            hash ^= static_cast<uint8_t>(v);
            hash *= FNV_PRIME;
            hash ^= static_cast<uint8_t>(v >> 8);
            hash *= FNV_PRIME;
            hash ^= static_cast<uint8_t>(v >> 16);
            hash *= FNV_PRIME;
            hash ^= static_cast<uint8_t>(v >> 24);
            hash *= FNV_PRIME;

            // Serialize and hash levels[i]
            uint16_t w = levels[i];
            hash ^= static_cast<uint8_t>(w);
            hash *= FNV_PRIME;
            hash ^= static_cast<uint8_t>(w >> 8);
            hash *= FNV_PRIME;

            // Serialize and hash lens[i]
            w = lens[i];
            hash ^= static_cast<uint8_t>(w);
            hash *= FNV_PRIME;
            hash ^= static_cast<uint8_t>(w >> 8);
            hash *= FNV_PRIME;
        }
        final_hash = hash;
    }
    return final_hash;
}