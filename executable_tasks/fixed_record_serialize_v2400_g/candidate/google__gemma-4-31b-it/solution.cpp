#include "interface.h"

#include <vector>
#include <cstring>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    
    const size_t num_records = ids.size();
    const size_t record_size = 4 + 4 + 2 + 2; // 12 bytes
    const size_t total_bytes = num_records * record_size;
    
    // Pre-allocate buffer to avoid repeated allocations
    std::vector<uint8_t> buffer(total_bytes);
    uint8_t* ptr = buffer.data();

    uint64_t final_hash = 0;
    const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* current = ptr;
        for (size_t i = 0; i < num_records; ++i) {
            uint32_t id = ids[i];
            uint32_t t = ts[i];
            uint16_t lvl = levels[i];
            uint16_t len = lens[i];

            // Manual serialization to avoid function call overhead and push_back
            *current++ = static_cast<uint8_t>(id);
            *current++ = static_cast<uint8_t>(id >> 8);
            *current++ = static_cast<uint8_t>(id >> 16);
            *current++ = static_cast<uint8_t>(id >> 24);

            *current++ = static_cast<uint8_t>(t);
            *current++ = static_cast<uint8_t>(t >> 8);
            *current++ = static_cast<uint8_t>(t >> 16);
            *current++ = static_cast<uint8_t>(t >> 24);

            *current++ = static_cast<uint8_t>(lvl);
            *current++ = static_cast<uint8_t>(lvl >> 8);

            *current++ = static_cast<uint8_t>(len);
            *current++ = static_cast<uint8_t>(len >> 8);
        }

        // Compute checksum on the filled buffer
        uint64_t hash = FNV_OFFSET_BASIS;
        for (size_t i = 0; i < total_bytes; ++i) {
            hash ^= static_cast<uint64_t>(buffer[i]);
            hash *= FNV_PRIME;
        }
        final_hash = hash;
    }

    return final_hash;
}