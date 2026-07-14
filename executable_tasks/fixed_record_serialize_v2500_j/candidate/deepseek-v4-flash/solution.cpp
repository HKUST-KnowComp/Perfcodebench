#include "interface.h"

#include <cstdint>

namespace {

constexpr uint64_t kFNVOffset = 1469598103934665603ULL;
constexpr uint64_t kFNVPrime = 1099511628211ULL;

// Compute FNV-1a hash from a uint32_t in little-endian byte order
inline void fnv_32(uint64_t& h, uint32_t v) {
    h ^= static_cast<uint8_t>(v);
    h *= kFNVPrime;
    h ^= static_cast<uint8_t>(v >> 8);
    h *= kFNVPrime;
    h ^= static_cast<uint8_t>(v >> 16);
    h *= kFNVPrime;
    h ^= static_cast<uint8_t>(v >> 24);
    h *= kFNVPrime;
}

// Compute FNV-1a hash from a uint16_t in little-endian byte order
inline void fnv_16(uint64_t& h, uint16_t v) {
    h ^= static_cast<uint8_t>(v);
    h *= kFNVPrime;
    h ^= static_cast<uint8_t>(v >> 8);
    h *= kFNVPrime;
}

} // anonymous namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    std::size_t n = ids.size();
    const uint32_t* id_ptr = ids.data();
    const uint32_t* ts_ptr = ts.data();
    const uint16_t* lvl_ptr = levels.data();
    const uint16_t* len_ptr = lens.data();

    uint64_t final_hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = kFNVOffset;
        for (std::size_t i = 0; i < n; ++i) {
            fnv_32(h, id_ptr[i]);
            fnv_32(h, ts_ptr[i]);
            fnv_16(h, lvl_ptr[i]);
            fnv_16(h, len_ptr[i]);
        }
        final_hash = h;
    }
    return final_hash;
}
