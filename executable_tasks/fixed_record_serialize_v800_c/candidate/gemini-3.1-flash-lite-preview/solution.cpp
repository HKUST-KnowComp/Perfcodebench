#include "interface.h"
#include <vector>
#include <cstring>

#pragma pack(push, 1)
struct Record {
    uint32_t id;
    uint32_t ts;
    uint16_t level;
    uint16_t len;
};
#pragma pack(pop)

uint64_t serialize_records_checksum(const std::vector<uint32_t>& ids, const std::vector<uint32_t>& ts, const std::vector<uint16_t>& levels, const std::vector<uint16_t>& lens, int iters) {
    size_t n = ids.size();
    std::vector<Record> buffer(n);
    for (size_t i = 0; i < n; ++i) {
        buffer[i] = {ids[i], ts[i], levels[i], lens[i]};
    }

    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* data = reinterpret_cast<const uint8_t*>(buffer.data());
    size_t total_bytes = n * sizeof(Record);

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_hash = 1469598103934665603ULL;
        for (size_t i = 0; i < total_bytes; ++i) {
            current_hash ^= static_cast<uint64_t>(data[i]);
            current_hash *= 1099511628211ULL;
        }
        hash = current_hash;
    }
    return hash;
}