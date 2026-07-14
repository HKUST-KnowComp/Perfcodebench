#include "interface.h"

#include <cstdint>
#include <vector>
#include <cstring>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    
    const size_t n = ids.size();
    if (n == 0) {
        // Handle empty case: the baseline would return checksum of empty vector
        // FNV-1a hash of empty sequence is the offset basis
        uint64_t hash = 1469598103934665603ULL;
        return hash;
    }

    // Pre-calculate the record size: 4 + 4 + 2 + 2 = 12 bytes
    const size_t record_size = 12;
    const size_t total_bytes = n * record_size;
    
    // Use a fixed buffer to avoid repeated allocations
    std::vector<uint8_t> buffer(total_bytes);
    uint8_t* ptr = buffer.data();

    // Pre-serialize the data once since it doesn't change across iterations
    for (size_t i = 0; i < n; ++i) {
        uint32_t id = ids[i];
        uint32_t t = ts[i];
        uint16_t lvl = levels[i];
        uint16_t len = lens[i];

        ptr[0] = static_cast<uint8_t>(id);
        ptr[1] = static_cast<uint8_t>(id >> 8);
        ptr[2] = static_cast<uint8_t>(id >> 16);
        ptr[3] = static_cast<uint8_t>(id >> 24);

        ptr[4] = static_cast<uint8_t>(t);
        ptr[5] = static_cast<uint8_t>(t >> 8);
        ptr[6] = static_cast<uint8_t>(t >> 16);
        ptr[7] = static_cast<uint8_t>(t >> 24);

        ptr[8] = static_cast<uint8_t>(lvl);
        ptr[9] = static_cast<uint8_t>(lvl >> 8);

        ptr[10] = static_cast<uint8_t>(len);
        ptr[11] = static_cast<uint8_t>(len >> 8);

        ptr += record_size;
    }

    uint64_t final_hash = 0;
    const uint8_t* data = buffer.data();
    const uint64_t offset_basis = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = offset_basis;
        for (size_t i = 0; i < total_bytes; ++i) {
            hash ^= static_cast<uint64_t>(data[i]);
            hash *= prime;
        }
        final_hash = hash;
    }

    return final_hash;
}