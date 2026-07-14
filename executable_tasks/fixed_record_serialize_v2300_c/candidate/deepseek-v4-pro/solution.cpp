#include "interface.h"

#include <cstdint>
#include <vector>

namespace {
    constexpr uint64_t init_hash = 1469598103934665603ULL;
    constexpr uint64_t multiplier = 1099511628211ULL;

    inline void hash_byte(uint64_t& h, uint8_t b) {
        h ^= b;
        h *= multiplier;
    }
}

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    if (iters <= 0) return 0;
    uint64_t hash = init_hash;
    size_t n = ids.size();
    for (size_t i = 0; i < n; ++i) {
        uint32_t id = ids[i];
        hash_byte(hash, static_cast<uint8_t>(id));
        hash_byte(hash, static_cast<uint8_t>(id >> 8));
        hash_byte(hash, static_cast<uint8_t>(id >> 16));
        hash_byte(hash, static_cast<uint8_t>(id >> 24));

        uint32_t t = ts[i];
        hash_byte(hash, static_cast<uint8_t>(t));
        hash_byte(hash, static_cast<uint8_t>(t >> 8));
        hash_byte(hash, static_cast<uint8_t>(t >> 16));
        hash_byte(hash, static_cast<uint8_t>(t >> 24));

        uint16_t level = levels[i];
        hash_byte(hash, static_cast<uint8_t>(level));
        hash_byte(hash, static_cast<uint8_t>(level >> 8));

        uint16_t len = lens[i];
        hash_byte(hash, static_cast<uint8_t>(len));
        hash_byte(hash, static_cast<uint8_t>(len >> 8));
    }
    return hash;
}