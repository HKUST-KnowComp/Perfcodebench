#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstring>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    if (iters <= 0) return 0;
    size_t n = ids.size();
    size_t buf_size = n * 12;
    std::vector<uint8_t> buf(buf_size);
    uint8_t* p = buf.data();

    for (size_t i = 0; i < n; ++i) {
        uint32_t id = ids[i];
        uint32_t t = ts[i];
        uint16_t lev = levels[i];
        uint16_t len = lens[i];
        *p++ = id; *p++ = id >> 8; *p++ = id >> 16; *p++ = id >> 24;
        *p++ = t; *p++ = t >> 8; *p++ = t >> 16; *p++ = t >> 24;
        *p++ = lev; *p++ = lev >> 8;
        *p++ = len; *p++ = len >> 8;
    }

    uint64_t hash = 1469598103934665603ULL;
    uint64_t C = 1099511628211ULL;
    const uint8_t* data = buf.data();
    size_t len = buf_size;
    size_t i = 0;
    for (; i + 7 < len; i += 8) {
        uint64_t val;
        std::memcpy(&val, data + i, 8);
        hash ^= (val & 0xFF); hash *= C;
        hash ^= ((val >> 8) & 0xFF); hash *= C;
        hash ^= ((val >> 16) & 0xFF); hash *= C;
        hash ^= ((val >> 24) & 0xFF); hash *= C;
        hash ^= ((val >> 32) & 0xFF); hash *= C;
        hash ^= ((val >> 40) & 0xFF); hash *= C;
        hash ^= ((val >> 48) & 0xFF); hash *= C;
        hash ^= ((val >> 56) & 0xFF); hash *= C;
    }
    for (; i < len; ++i) {
        hash ^= data[i]; hash *= C;
    }

    return hash;
}