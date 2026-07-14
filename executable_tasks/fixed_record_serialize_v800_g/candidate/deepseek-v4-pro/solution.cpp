#include "interface.h"

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    if (iters <= 0) return 0;
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t mul = 1099511628211ULL;
    const std::size_t n = ids.size();
    for (std::size_t i = 0; i < n; ++i) {
        uint32_t id = ids[i];
        hash ^= static_cast<uint8_t>(id);            hash *= mul;
        hash ^= static_cast<uint8_t>(id >> 8);       hash *= mul;
        hash ^= static_cast<uint8_t>(id >> 16);      hash *= mul;
        hash ^= static_cast<uint8_t>(id >> 24);      hash *= mul;
        uint32_t t = ts[i];
        hash ^= static_cast<uint8_t>(t);             hash *= mul;
        hash ^= static_cast<uint8_t>(t >> 8);        hash *= mul;
        hash ^= static_cast<uint8_t>(t >> 16);       hash *= mul;
        hash ^= static_cast<uint8_t>(t >> 24);       hash *= mul;
        uint16_t lv = levels[i];
        hash ^= static_cast<uint8_t>(lv);            hash *= mul;
        hash ^= static_cast<uint8_t>(lv >> 8);       hash *= mul;
        uint16_t ln = lens[i];
        hash ^= static_cast<uint8_t>(ln);            hash *= mul;
        hash ^= static_cast<uint8_t>(ln >> 8);       hash *= mul;
    }
    return hash;
}
