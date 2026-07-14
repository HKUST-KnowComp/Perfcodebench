#include "interface.h"
#include <vector>
#include <cstring>
#include <cstdint>

namespace {

inline uint64_t fnv1a(const uint8_t* data, std::size_t len) {
    uint64_t h = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    for (std::size_t i = 0; i < len; ++i) {
        h ^= static_cast<uint64_t>(data[i]);
        h *= prime;
    }
    return h;
}

} // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {

    const std::size_t n = ids.size();
    const std::size_t record_bytes = 12;
    const std::size_t total_bytes = n * record_bytes;

    // Pre-allocate buffer once; reuse across iterations
    std::vector<uint8_t> buf(total_bytes);

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* p = buf.data();
        for (std::size_t i = 0; i < n; ++i) {
            // ids[i]  (4 bytes)
            std::memcpy(p, &ids[i], 4);
            p += 4;
            // ts[i]   (4 bytes)
            std::memcpy(p, &ts[i], 4);
            p += 4;
            // levels[i] (2 bytes)
            std::memcpy(p, &levels[i], 2);
            p += 2;
            // lens[i]   (2 bytes)
            std::memcpy(p, &lens[i], 2);
            p += 2;
        }
        hash = fnv1a(buf.data(), total_bytes);
    }
    return hash;
}