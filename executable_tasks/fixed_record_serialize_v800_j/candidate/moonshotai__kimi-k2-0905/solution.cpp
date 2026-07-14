#include "interface.h"
#include <vector>
#include <cstddef>
#include <cstring>

namespace {

uint64_t checksum_bytes(const uint8_t* __restrict p, std::size_t len) {
    const uint64_t FNV_P = 1099511628211ULL;
    uint64_t h0 = 1469598103934665603ULL;
    std::size_t i = 0;
    for (; i + 8 <= len; i += 8) {
        h0 ^= static_cast<uint64_t>(p[i+0]); h0 *= FNV_P;
        h0 ^= static_cast<uint64_t>(p[i+1]); h0 *= FNV_P;
        h0 ^= static_cast<uint64_t>(p[i+2]); h0 *= FNV_P;
        h0 ^= static_cast<uint64_t>(p[i+3]); h0 *= FNV_P;
        h0 ^= static_cast<uint64_t>(p[i+4]); h0 *= FNV_P;
        h0 ^= static_cast<uint64_t>(p[i+5]); h0 *= FNV_P;
        h0 ^= static_cast<uint64_t>(p[i+6]); h0 *= FNV_P;
        h0 ^= static_cast<uint64_t>(p[i+7]); h0 *= FNV_P;
    }
    for (; i < len; ++i) {
        h0 ^= static_cast<uint64_t>(p[i]);
        h0 *= FNV_P;
    }
    return h0;
}

inline void store_le32(uint8_t* dst, uint32_t v) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    *reinterpret_cast<uint32_t*>(dst) = v;
#else
    dst[0] = static_cast<uint8_t>(v);
    dst[1] = static_cast<uint8_t>(v >> 8);
    dst[2] = static_cast<uint8_t>(v >> 16);
    dst[3] = static_cast<uint8_t>(v >> 24);
#endif
}

inline void store_le16(uint8_t* dst, uint16_t v) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    *reinterpret_cast<uint16_t*>(dst) = v;
#else
    dst[0] = static_cast<uint8_t>(v);
    dst[1] = static_cast<uint8_t>(v >> 8);
#endif
}

}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    const std::size_t n = ids.size();
    const std::size_t rec_size = 12;               // 4+4+2+2
    const std::size_t total = n * rec_size;
    std::vector<uint8_t> out;
    out.resize(total);

    uint8_t* __restrict base = out.data();
    for (std::size_t i = 0; i < n; ++i) {
        uint8_t* dst = base + i * rec_size;
        store_le32(dst, ids[i]);
        store_le32(dst + 4, ts[i]);
        store_le16(dst + 8, levels[i]);
        store_le16(dst + 10, lens[i]);
    }

    uint64_t h = checksum_bytes(base, total);
    for (int iter = 1; iter < iters; ++iter) {
        h = checksum_bytes(base, total);
    }
    return h;
}