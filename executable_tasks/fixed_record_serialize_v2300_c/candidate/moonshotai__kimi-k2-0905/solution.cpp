#include "interface.h"
#include <vector>
#include <cstring>

namespace {

inline uint64_t checksum_bytes(const uint8_t* data, std::size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* end = data + len;
    while (data != end) {
        hash ^= *data++;
        hash *= 1099511628211ULL;
    }
    return hash;
}

struct alignas(1) Record {
    uint32_t id;
    uint32_t ts;
    uint16_t level;
    uint16_t len;
};

} // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters)
{
    const std::size_t n = ids.size();
    const std::size_t record_size = sizeof(Record);
    const std::size_t total_size = n * record_size;

    std::vector<uint8_t> buffer(total_size);

    Record* dst = reinterpret_cast<Record*>(buffer.data());
    for (std::size_t i = 0; i < n; ++i) {
        dst[i].id    = ids[i];
        dst[i].ts    = ts[i];
        dst[i].level = levels[i];
        dst[i].len   = lens[i];
    }

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = checksum_bytes(buffer.data(), total_size);
    }
    return hash;
}