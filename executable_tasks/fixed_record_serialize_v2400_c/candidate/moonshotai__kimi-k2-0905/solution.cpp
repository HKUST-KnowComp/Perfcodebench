#include "interface.h"
#include <vector>
#include <cstring>

namespace {

inline uint64_t hash_u64(uint64_t h, uint64_t x) {
    constexpr uint64_t kMul = 0x9ddfea08eb382d69ULL;
    uint64_t v = x ^ h;
    v ^= v >> 32;
    v *= kMul;
    v ^= v >> 28;
    v *= kMul;
    v ^= v >> 29;
    return v;
}

inline uint64_t checksum_bytes(const std::vector<uint8_t>& bytes) {
    const uint8_t* data = bytes.data();
    const std::size_t len = bytes.size();
    uint64_t hash = 0x9e3779b97f4a7c15ULL;
    std::size_t i = 0;
    for (; i + 8 <= len; i += 8) {
        uint64_t chunk;
        std::memcpy(&chunk, data + i, 8);
        hash = hash_u64(hash, chunk);
    }
    for (; i < len; ++i) {
        hash = hash_u64(hash, data[i]);
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
    const std::size_t n = ids.size();
    const std::size_t record_bytes = 4 + 4 + 2 + 2;
    const std::size_t total_bytes = n * record_bytes;

    std::vector<uint8_t> out;
    out.resize(total_bytes);

    uint8_t* dst = out.data();
    for (std::size_t i = 0; i < n; ++i) {
        uint32_t id = ids[i];
        uint32_t t  = ts[i];
        uint16_t lvl = levels[i];
        uint16_t ln  = lens[i];

        std::memcpy(dst, &id, 4); dst += 4;
        std::memcpy(dst, &t,  4); dst += 4;
        std::memcpy(dst, &lvl, 2); dst += 2;
        std::memcpy(dst, &ln,  2); dst += 2;
    }

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = checksum_bytes(out);
    }
    return hash;
}