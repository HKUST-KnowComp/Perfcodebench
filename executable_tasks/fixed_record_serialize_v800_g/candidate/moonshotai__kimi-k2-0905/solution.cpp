#include "interface.h"

#include <vector>
#include <cstring>
#include <cstdint>

namespace {

inline uint64_t checksum_bytes(const uint8_t* data, std::size_t len) {
    const uint64_t prime = 1099511628211ULL;
    uint64_t hash = 1469598103934665603ULL;

    // Process 8 bytes at a time
    const uint8_t* end8 = data + (len & ~std::size_t(7));
    while (data < end8) {
        uint64_t chunk;
        std::memcpy(&chunk, data, 8);
        hash ^= chunk;
        hash *= prime;
        data += 8;
    }

    // Tail bytes
    len &= 7;
    while (len--) {
        hash ^= *data++;
        hash *= prime;
    }
    return hash;
}

}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {

    const std::size_t record_size = 4 + 4 + 2 + 2;  // 12 bytes
    const std::size_t total_size = ids.size() * record_size;
    if (total_size == 0) return 0;

    std::vector<uint8_t> out;
    out.resize(total_size);

    uint8_t* dst = out.data();
    const uint32_t* id  = ids.data();
    const uint32_t* t   = ts.data();
    const uint16_t* lvl = levels.data();
    const uint16_t* len = lens.data();

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* pos = dst;
        for (std::size_t i = 0, n = ids.size(); i < n; ++i) {
            std::memcpy(pos,      &id[i],  4); pos += 4;
            std::memcpy(pos,      &t[i],   4); pos += 4;
            std::memcpy(pos,      &lvl[i], 2); pos += 2;
            std::memcpy(pos,      &len[i], 2); pos += 2;
        }
        hash = checksum_bytes(dst, total_size);
    }
    return hash;
}