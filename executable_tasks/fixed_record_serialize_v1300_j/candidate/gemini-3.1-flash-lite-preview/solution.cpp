#include "interface.h"
#include <vector>
#include <cstring>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    
    size_t n = ids.size();
    size_t record_size = 12; // 4 + 4 + 2 + 2
    std::vector<uint8_t> buffer(n * record_size);
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* ptr = buffer.data();
        for (size_t i = 0; i < n; ++i) {
            std::memcpy(ptr, &ids[i], 4);
            std::memcpy(ptr + 4, &ts[i], 4);
            std::memcpy(ptr + 8, &levels[i], 2);
            std::memcpy(ptr + 10, &lens[i], 2);
            ptr += 12;
        }

        uint64_t hash = 1469598103934665603ULL;
        const uint8_t* data = buffer.data();
        size_t total_bytes = n * record_size;
        for (size_t i = 0; i < total_bytes; ++i) {
            hash ^= static_cast<uint64_t>(data[i]);
            hash *= 1099511628211ULL;
        }
        final_hash = hash;
    }
    
    return final_hash;
}