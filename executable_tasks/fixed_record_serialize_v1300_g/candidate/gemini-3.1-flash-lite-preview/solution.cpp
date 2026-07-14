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
    size_t record_size = sizeof(uint32_t) * 2 + sizeof(uint16_t) * 2;
    size_t total_size = n * record_size;
    
    std::vector<uint8_t> buffer(total_size);
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* ptr = buffer.data();
        for (size_t i = 0; i < n; ++i) {
            std::memcpy(ptr, &ids[i], 4);
            ptr += 4;
            std::memcpy(ptr, &ts[i], 4);
            ptr += 4;
            std::memcpy(ptr, &levels[i], 2);
            ptr += 2;
            std::memcpy(ptr, &lens[i], 2);
            ptr += 2;
        }

        uint64_t current_hash = 1469598103934665603ULL;
        const uint8_t* data = buffer.data();
        for (size_t i = 0; i < total_size; ++i) {
            current_hash ^= static_cast<uint64_t>(data[i]);
            current_hash *= 1099511628211ULL;
        }
        hash = current_hash;
    }
    
    return hash;
}