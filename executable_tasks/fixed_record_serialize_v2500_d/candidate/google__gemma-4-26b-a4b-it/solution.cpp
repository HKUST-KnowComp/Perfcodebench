#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstring>

namespace {

// FNV-1a constants
constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline uint64_t compute_fnv1a(const uint8_t* data, size_t size) {
    uint64_t hash = FNV_OFFSET_BASIS;
    for (size_t i = 0; i < size; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= FNV_PRIME;
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
    
    const size_t num_records = ids.size();
    if (num_records == 0) {
        // If no records, the buffer is empty. FNV-1a of empty is offset basis.
        // However, the loop logic implies we return the last hash calculated.
        // Based on baseline, if iters > 0 and size is 0, hash is checksum_bytes(empty).
        return (iters > 0) ? FNV_OFFSET_BASIS : 0;
    }

    // Each record size: 4 (id) + 4 (ts) + 2 (level) + 2 (len) = 12 bytes
    const size_t record_size = 12;
    const size_t total_size = num_records * record_size;
    
    // Pre-allocate buffer to avoid reallocations
    std::vector<uint8_t> buffer(total_size);
    uint8_t* ptr = buffer.data();
    
    uint64_t last_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* curr = ptr;
        for (size_t i = 0; i < num_records; ++i) {
            // Manual unrolling/writing to ensure little-endian as per baseline
            uint32_t id = ids[i];
            uint32_t t = ts[i];
            uint16_t l = levels[i];
            uint16_t ln = lens[i];

            // Write ID
            *curr++ = static_cast<uint8_t>(id);
            *curr++ = static_cast<uint8_t>(id >> 8);
            *curr++ = static_cast<uint8_t>(id >> 16);
            *curr++ = static_cast<uint8_t>(id >> 24);

            // Write TS
            *curr++ = static_cast<uint8_t>(t);
            *curr++ = static_cast<uint8_t>(t >> 8);
            *curr++ = static_cast<uint8_t>(t >> 16);
            *curr++ = static_cast<uint8_t>(t >> 24);

            // Write Level
            *curr++ = static_cast<uint8_t>(l);
            *curr++ = static_cast<uint8_t>(l >> 8);

            // Write Lens
            *curr++ = static_cast<uint8_t>(ln);
            *curr++ = static_cast<uint8_t>(ln >> 8);
        }
        last_hash = compute_fnv1a(ptr, total_size);
    }

    return last_hash;
}
