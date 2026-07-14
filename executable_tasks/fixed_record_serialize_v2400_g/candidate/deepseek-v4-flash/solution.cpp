#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

inline uint64_t fnv1a_update(uint64_t hash, uint8_t byte) {
    hash ^= static_cast<uint64_t>(byte);
    hash *= 1099511628211ULL;
    return hash;
}

}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    uint64_t hash = 0;
    const uint64_t offset_basis = 1469598103934665603ULL;
    const std::size_t n = ids.size();
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = offset_basis;
        for (std::size_t i = 0; i < n; ++i) {
            uint32_t id = ids[i];
            h = fnv1a_update(h, static_cast<uint8_t>(id));
            h = fnv1a_update(h, static_cast<uint8_t>(id >> 8));
            h = fnv1a_update(h, static_cast<uint8_t>(id >> 16));
            h = fnv1a_update(h, static_cast<uint8_t>(id >> 24));

            uint32_t t = ts[i];
            h = fnv1a_update(h, static_cast<uint8_t>(t));
            h = fnv1a_update(h, static_cast<uint8_t>(t >> 8));
            h = fnv1a_update(h, static_cast<uint8_t>(t >> 16));
            h = fnv1a_update(h, static_cast<uint8_t>(t >> 24));

            uint16_t level = levels[i];
            h = fnv1a_update(h, static_cast<uint8_t>(level));
            h = fnv1a_update(h, static_cast<uint8_t>(level >> 8));

            uint16_t len = lens[i];
            h = fnv1a_update(h, static_cast<uint8_t>(len));
            h = fnv1a_update(h, static_cast<uint8_t>(len >> 8));
        }
        hash = h;
    }
    return hash;
}