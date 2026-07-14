#include "interface.h"
#include <cstdint>
#include <vector>

namespace {
constexpr uint64_t FNV1A_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV1A_PRIME = 1099511628211ULL;

inline uint64_t fnv1a_step(uint64_t hash, uint8_t byte) {
    hash ^= byte;
    hash *= FNV1A_PRIME;
    return hash;
}
}

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    const std::size_t n = ids.size();
    const uint32_t* id_ptr = ids.data();
    const uint32_t* ts_ptr = ts.data();
    const uint16_t* level_ptr = levels.data();
    const uint16_t* len_ptr = lens.data();

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV1A_OFFSET;
        for (std::size_t i = 0; i < n; ++i) {
            uint32_t id = id_ptr[i];
            hash = fnv1a_step(hash, id & 0xff);
            hash = fnv1a_step(hash, (id >> 8) & 0xff);
            hash = fnv1a_step(hash, (id >> 16) & 0xff);
            hash = fnv1a_step(hash, (id >> 24) & 0xff);
            uint32_t t = ts_ptr[i];
            hash = fnv1a_step(hash, t & 0xff);
            hash = fnv1a_step(hash, (t >> 8) & 0xff);
            hash = fnv1a_step(hash, (t >> 16) & 0xff);
            hash = fnv1a_step(hash, (t >> 24) & 0xff);
            uint16_t level = level_ptr[i];
            hash = fnv1a_step(hash, level & 0xff);
            hash = fnv1a_step(hash, (level >> 8) & 0xff);
            uint16_t len = len_ptr[i];
            hash = fnv1a_step(hash, len & 0xff);
            hash = fnv1a_step(hash, (len >> 8) & 0xff);
        }
    }
    return hash;
}