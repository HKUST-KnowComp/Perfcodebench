#include "interface.h"
#include <vector>
#include <cstdint>

static inline void write_u32(uint8_t*& dst, uint32_t v) {
    *dst++ = static_cast<uint8_t>(v);
    *dst++ = static_cast<uint8_t>(v >> 8);
    *dst++ = static_cast<uint8_t>(v >> 16);
    *dst++ = static_cast<uint8_t>(v >> 24);
}

static inline void write_u16(uint8_t*& dst, uint16_t v) {
    *dst++ = static_cast<uint8_t>(v);
    *dst++ = static_cast<uint8_t>(v >> 8);
}

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters)
{
    const size_t rec_count = ids.size();
    const size_t bytes_per_rec = 4 + 4 + 2 + 2; // 12 bytes
    const size_t total_size = rec_count * bytes_per_rec;

    std::vector<uint8_t> out;
    out.resize(total_size);

    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* ptr = out.data();
        for (size_t i = 0; i < rec_count; ++i) {
            write_u32(ptr, ids[i]);
            write_u32(ptr, ts[i]);
            write_u16(ptr, levels[i]);
            write_u16(ptr, lens[i]);
        }

        const uint64_t* p64 = reinterpret_cast<const uint64_t*>(out.data());
        const uint8_t*  p8  = out.data();

        const size_t rem = total_size % 8;
        const size_t n64 = total_size / 8;

        uint64_t h64 = 1469598103934665603ULL;
        for (size_t i = 0; i < n64; ++i) {
            uint64_t word = p64[i];
            h64 ^= word;
            h64 *= 1099511628211ULL;
        }

        for (size_t i = total_size - rem; i < total_size; ++i) {
            h64 ^= static_cast<uint64_t>(p8[i]);
            h64 *= 1099511628211ULL;
        }
        hash = h64;
    }
    return hash;
}