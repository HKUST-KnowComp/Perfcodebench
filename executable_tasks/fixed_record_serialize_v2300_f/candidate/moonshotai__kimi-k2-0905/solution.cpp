#include "interface.h"
#include <vector>
#include <cstring>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters)
{
    const size_t n = ids.size();
    const size_t record_bytes = 12;            // 4+4+2+2
    const size_t total_bytes  = n * record_bytes;

    // Pre-allocate once; iters loop will overwrite in-place.
    std::vector<uint8_t> buffer(total_bytes);

    // FNV-1a constants
    const uint64_t fnv_offset = 1469598103934665603ULL;
    const uint64_t fnv_prime  = 1099511628211ULL;

    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* dst = buffer.data();
        uint64_t h = fnv_offset;

        for (size_t i = 0; i < n; ++i) {
            // Layout: id(4) | ts(4) | level(2) | len(2)
            uint32_t id    = ids[i];
            uint32_t t     = ts[i];
            uint16_t level = levels[i];
            uint16_t len   = lens[i];

            // Store little-endian 32/16-bit values
            memcpy(dst,      &id,    4);
            memcpy(dst + 4,  &t,     4);
            memcpy(dst + 8,  &level, 2);
            memcpy(dst + 10, &len,   2);

            // Update checksum while data are in L1
            for (size_t k = 0; k < record_bytes; ++k) {
                h ^= static_cast<uint64_t>(dst[k]);
                h *= fnv_prime;
            }
            dst += record_bytes;
        }
        hash = h;
    }
    return hash;
}