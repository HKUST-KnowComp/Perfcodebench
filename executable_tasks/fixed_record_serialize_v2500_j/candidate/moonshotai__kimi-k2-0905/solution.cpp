#include "interface.h"
#include <cstdint>
#include <vector>
#include <cstring>

namespace {

inline uint64_t update_hash(uint64_t h, uint8_t b) {
    h ^= static_cast<uint64_t>(b);
    h *= 1099511628211ULL;
    return h;
}

inline uint64_t update_hash_u32(uint64_t h, uint32_t v) {
    h = update_hash(h, static_cast<uint8_t>(v));
    h = update_hash(h, static_cast<uint8_t>(v >> 8));
    h = update_hash(h, static_cast<uint8_t>(v >> 16));
    h = update_hash(h, static_cast<uint8_t>(v >> 24));
    return h;
}

inline uint64_t update_hash_u16(uint64_t h, uint16_t v) {
    h = update_hash(h, static_cast<uint8_t>(v));
    h = update_hash(h, static_cast<uint8_t>(v >> 8));
    return h;
}

}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {

    const size_t n = ids.size();
    const size_t bytes_per_record = 4 + 4 + 2 + 2;
    const size_t total_bytes = n * bytes_per_record;

    // single reusable buffer
    std::vector<uint8_t> buffer(total_bytes);

    uint64_t hash = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* dst = buffer.data();
        uint64_t h = 1469598103934665603ULL;

        for (size_t i = 0; i < n; ++i) {
            uint32_t id = ids[i];
            uint32_t t  = ts[i];
            uint16_t lvl = levels[i];
            uint16_t len = lens[i];

            *dst++ = static_cast<uint8_t>(id);
            *dst++ = static_cast<uint8_t>(id >> 8);
            *dst++ = static_cast<uint8_t>(id >> 16);
            *dst++ = static_cast<uint8_t>(id >> 24);
            h = update_hash_u32(h, id);

            *dst++ = static_cast<uint8_t>(t);
            *dst++ = static_cast<uint8_t>(t >> 8);
            *dst++ = static_cast<uint8_t>(t >> 16);
            *dst++ = static_cast<uint8_t>(t >> 24);
            h = update_hash_u32(h, t);

            *dst++ = static_cast<uint8_t>(lvl);
            *dst++ = static_cast<uint8_t>(lvl >> 8);
            h = update_hash_u16(h, lvl);

            *dst++ = static_cast<uint8_t>(len);
            *dst++ = static_cast<uint8_t>(len >> 8);
            h = update_hash_u16(h, len);
        }
        hash = h;
    }
    return hash;
}