#include "interface.h"
#include <cstring>
#include <cstdint>

namespace {

inline uint64_t checksum_bytes(const uint8_t* ptr, size_t n) {
    uint64_t hash = 14695981039346656037ULL;
    const uint8_t* end = ptr + n;
    while (ptr != end) {
        hash ^= static_cast<uint64_t>(*ptr++);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {

    const size_t recs = ids.size();
    const size_t row_bytes = recs * 12;

    alignas(64) static uint8_t buf[1 << 20]; // 1 MB scratch, fits typical sizes
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* out = buf;
        for (size_t i = 0; i < recs; ++i) {
            uint32_t id  = ids[i];
            uint32_t t   = ts[i];
            uint16_t lvl = levels[i];
            uint16_t ln  = lens[i];

            std::memcpy(out, &id, 4);   out += 4;
            std::memcpy(out, &t, 4);     out += 4;
            std::memcpy(out, &lvl, 2);   out += 2;
            std::memcpy(out, &ln, 2);    out += 2;
        }
        hash = checksum_bytes(buf, row_bytes);
    }
    return hash;
}