#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstring>

namespace {

// The baseline uses a specific FNV-1a implementation.
// To maintain correctness, we must replicate the exact byte-by-byte logic.
inline uint64_t compute_fnv1a(const uint8_t* data, size_t size) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < size; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
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
    
    if (ids.empty()) return 0;

    const size_t n = ids.size();
    // Each record is 4 (id) + 4 (ts) + 2 (level) + 2 (len) = 12 bytes
    const size_t record_size = 12;
    const size_t total_size = n * record_size;
    
    // Pre-allocate buffer to avoid reallocations
    std::vector<uint8_t> buffer(total_size);
    uint8_t* ptr = buffer.data();
    
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        ptr = buffer.data();
        for (size_t i = 0; i < n; ++i) {
            // Manual unrolling of the little-endian writes to match baseline behavior
            uint32_t id = ids[i];
            uint32_t t = ts[i];
            uint16_t l = levels[i];
            uint16_t ln = lens[i];

            // Write ID
            *ptr++ = static_cast<uint8_t>(id);
            *ptr++ = static_cast<uint8_t>(id >> 8);
            *ptr++ = static_cast<uint8_t>(id >> 16);
            *ptr++ = static_cast<uint8_t>(id >> 24);

            // Write TS
            *ptr++ = static_cast<uint8_t>(t);
            *ptr++ = static_cast<uint8_t>(t >> 8);
            *ptr++ = static_cast<uint8_t>(t >> 16);
            *ptr++ = static_cast<uint8_t>(t >> 24);

            // Write Level
            *ptr++ = static_cast<uint8_t>(l);
            *ptr++ = static_cast<uint8_t>(l >> 8);

            // Write Lens
            *ptr++ = static_cast<uint8_t>(ln);
            *ptr++ = static_cast<uint8_t>(ln >> 8);
        }
        final_hash = compute_fnv1a(buffer.data(), total_size);
    }

    return final_hash;
}