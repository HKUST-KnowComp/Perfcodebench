#include "interface.h"

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    if (iters == 0) return 0;
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t mul = 1099511628211ULL;
    const size_t n = ids.size();
    for (size_t i = 0; i < n; ++i) {
        uint32_t id = ids[i];
        uint32_t t = ts[i];
        uint16_t lv = levels[i];
        uint16_t ln = lens[i];
        // id (little-endian bytes)
        hash ^= (id & 0xFF); hash *= mul;
        hash ^= ((id >> 8) & 0xFF); hash *= mul;
        hash ^= ((id >> 16) & 0xFF); hash *= mul;
        hash ^= ((id >> 24) & 0xFF); hash *= mul;
        // ts
        hash ^= (t & 0xFF); hash *= mul;
        hash ^= ((t >> 8) & 0xFF); hash *= mul;
        hash ^= ((t >> 16) & 0xFF); hash *= mul;
        hash ^= ((t >> 24) & 0xFF); hash *= mul;
        // level
        hash ^= (lv & 0xFF); hash *= mul;
        hash ^= ((lv >> 8) & 0xFF); hash *= mul;
        // lens
        hash ^= (ln & 0xFF); hash *= mul;
        hash ^= ((ln >> 8) & 0xFF); hash *= mul;
    }
    return hash;
}