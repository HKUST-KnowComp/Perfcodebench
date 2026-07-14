#include "interface.h"

#include <cstdint>
#include <vector>
#include <cstring>

namespace {

inline uint64_t read_u64(const uint8_t* p) {
    uint64_t v;
    std::memcpy(&v, p, sizeof(v));
    return v;
}

inline void write_u64(uint8_t* p, uint64_t v) {
    std::memcpy(p, &v, sizeof(v));
}

} // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {

    const size_t n = ids.size();
    if (n == 0 || iters == 0) return 0;

    const size_t record_size = 4 + 4 + 2 + 2;        // 12 bytes
    const size_t total_bytes = n * record_size;

    // Buffer reused for every iteration
    std::vector<uint8_t> buffer(total_bytes);
    uint8_t* out = buffer.data();

    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        // Serialize all records in one pass
        for (size_t i = 0; i < n; ++i) {
            uint8_t* dst = out + i * record_size;

            // Write 4 bytes id (little-endian)
            uint32_t id = ids[i];
            dst[0] = static_cast<uint8_t>(id);
            dst[1] = static_cast<uint8_t>(id >> 8);
            dst[2] = static_cast<uint8_t>(id >> 16);
            dst[3] = static_cast<uint8_t>(id >> 24);

            // Write 4 bytes ts (little-endian)
            uint32_t t = ts[i];
            dst[4] = static_cast<uint8_t>(t);
            dst[5] = static_cast<uint8_t>(t >> 8);
            dst[6] = static_cast<uint8_t>(t >> 16);
            dst[7] = static_cast<uint8_t>(t >> 24);

            // Write 2 bytes level (little-endian)
            uint16_t level = levels[i];
            dst[8]  = static_cast<uint8_t>(level);
            dst[9]  = static_cast<uint8_t>(level >> 8);

            // Write 2 bytes len (little-endian)
            uint16_t len = lens[i];
            dst[10] = static_cast<uint8_t>(len);
            dst[11] = static_cast<uint8_t>(len >> 8);
        }

        // Compute checksum inline
        uint64_t h = 1469598103934665603ULL;
        const uint8_t* ptr = out;
        const uint8_t* end = out + total_bytes;

        // 8-byte aligned loop
        const size_t aligned = total_bytes & ~size_t(7);
        for (size_t i = 0; i < aligned; i += 8) {
            uint64_t chunk = read_u64(ptr + i);
            for (int k = 0; k < 8; ++k) {
                h ^= (chunk & 0xFF);
                h *= 1099511628211ULL;
                chunk >>= 8;
            }
        }

        // Tail bytes
        for (size_t i = aligned; i < total_bytes; ++i) {
            h ^= uint64_t(ptr[i]);
            h *= 1099511628211ULL;
        }
        hash = h;
    }

    return hash;
}