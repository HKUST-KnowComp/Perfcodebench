#include "interface.h"
#include <cstdint>
#include <vector>
#include <cstddef>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    if (iters <= 0) return 0;
    
    const std::size_t n = ids.size();
    if (n == 0) return 1469598103934665603ULL;

    const uint32_t* p_ids = ids.data();
    const uint32_t* p_ts = ts.data();
    const uint16_t* p_levels = levels.data();
    const uint16_t* p_lens = lens.data();

    uint64_t hash = 1469598103934665603ULL;
    constexpr uint64_t prime = 1099511628211ULL;

    for (std::size_t i = 0; i < n; ++i) {
        uint32_t id = p_ids[i];
        uint32_t t = p_ts[i];
        uint16_t lev = p_levels[i];
        uint16_t ln = p_lens[i];

        hash ^= static_cast<uint8_t>(id); hash *= prime;
        hash ^= static_cast<uint8_t>(id >> 8); hash *= prime;
        hash ^= static_cast<uint8_t>(id >> 16); hash *= prime;
        hash ^= static_cast<uint8_t>(id >> 24); hash *= prime;

        hash ^= static_cast<uint8_t>(t); hash *= prime;
        hash ^= static_cast<uint8_t>(t >> 8); hash *= prime;
        hash ^= static_cast<uint8_t>(t >> 16); hash *= prime;
        hash ^= static_cast<uint8_t>(t >> 24); hash *= prime;

        hash ^= static_cast<uint8_t>(lev); hash *= prime;
        hash ^= static_cast<uint8_t>(lev >> 8); hash *= prime;

        hash ^= static_cast<uint8_t>(ln); hash *= prime;
        hash ^= static_cast<uint8_t>(ln >> 8); hash *= prime;
    }

    return hash;
}