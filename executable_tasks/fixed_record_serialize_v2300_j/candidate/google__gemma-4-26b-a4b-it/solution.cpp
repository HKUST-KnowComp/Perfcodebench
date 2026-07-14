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

    // Each record is 4 (id) + 4 (ts) + 2 (level) + 2 (len) = 12 bytes
    const size_t record_size = 12;
    const size_t total_bytes = num_records * record_size;
    
    // Pre-allocate buffer once to avoid reallocations
    std::vector<uint8_t> buffer(total_bytes);
    uint8_t* ptr = buffer.data();
    
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* curr = ptr;
        
        // Manual unrolling or simple loop for high-speed serialization
        for (size_t i = 0; i < num_records; ++i) {
            uint32_t id = ids[i];
            uint32_t t = ts[i];
            uint16_t l = levels[i];
            uint16_t ln = lens[i];

            // Write u32 id (little endian)
            curr[0] = static_cast<uint8_t>(id);
            curr[1] = static_cast<uint8_t>(id >> 8);
            curr[2] = static_cast<uint8_t>(id >> 16);
            curr[3] = static_cast<uint8_t>(id >> 24);
            
            // Write u32 ts (little endian)
            curr[4] = static_cast<uint8_t>(t);
            curr[5] = static_cast<uint8_t>(t >> 8);
            curr[6] = static_cast<uint8_t>(t >> 16);
            curr[7] = static_cast<uint8_t>(t >> 24);
            
            // Write u16 level (little endian)
            curr[8] = static_cast<uint8_t>(l);
            curr[9] = static_cast<uint8_t>(l >> 8);
            
            // Write u16 len (little endian)
            curr[10] = static_cast<uint8_t>(ln);
            curr[11] = static_cast<uint8_t>(ln >> 8);
            
            curr += record_size;
        }
        
        final_hash = compute_fnv1a(ptr, total_bytes);
    }

    return final_hash;
}