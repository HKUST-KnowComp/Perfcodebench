#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstring>

namespace {

// Optimized FNV-1a checksum implementation
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
    
    const size_t num_records = ids.size();
    if (num_records == 0) return 0;

    // Each record is 4 + 4 + 2 + 2 = 12 bytes
    const size_t record_size = 12;
    const size_t total_size = num_records * record_size;
    
    // Pre-allocate buffer to avoid reallocations
    std::vector<uint8_t> buffer(total_size);
    uint8_t* ptr = buffer.data();
    
    uint64_t last_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* curr = ptr;
        for (size_t i = 0; i < num_records; ++i) {
            // Manual unrolling of little-endian writes for maximum performance
            uint32_t id = ids[i];
            uint32_t t = ts[i];
            uint16_t l = levels[i];
            uint16_t ln = lens[i];

            // Write ID
            curr[0] = static_cast<uint8_t>(id);
            curr[1] = static_cast<uint8_t>(id >> 8);
            curr[2] = static_cast<uint8_t>(id >> 16);
            curr[3] = static_cast<uint8_t>(id >> 24);
            
            // Write TS
            curr[4] = static_cast<uint8_t>(t);
            curr[5] = static_cast<uint8_t>(t >> 8);
            curr[6] = static_cast<uint8_t>(t >> 16);
            curr[7] = static_cast<uint8_t>(t >> 24);

            // Write Level
            curr[8] = static_cast<uint8_t>(l);
            curr[9] = static_cast<uint8_t>(l >> 8);

            // Write Lens
            curr[10] = static_cast<uint8_t>(ln);
            curr[11] = static_cast<uint8_t>(ln >> 8);

            curr += record_size;
        }
        last_hash = compute_fnv1a(ptr, total_size);
    }

    return last_hash;
}