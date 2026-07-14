#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    const size_t n = ids.size();
    const size_t total_bytes = n * 12;
    std::vector<uint8_t> out;
    out.reserve(total_bytes);
    
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        out.resize(total_bytes);
        uint8_t* p = out.data();
        hash = FNV_OFFSET;
        
        for (size_t i = 0; i < n; ++i) {
            uint32_t id = ids[i];
            p[0] = static_cast<uint8_t>(id); hash ^= static_cast<uint64_t>(id & 0xFF); hash *= FNV_PRIME; ++p;
            p[0] = static_cast<uint8_t>(id >> 8); hash ^= static_cast<uint64_t>((id >> 8) & 0xFF); hash *= FNV_PRIME; ++p;
            p[0] = static_cast<uint8_t>(id >> 16); hash ^= static_cast<uint64_t>((id >> 16) & 0xFF); hash *= FNV_PRIME; ++p;
            p[0] = static_cast<uint8_t>(id >> 24); hash ^= static_cast<uint64_t>((id >> 24) & 0xFF); hash *= FNV_PRIME; ++p;

            uint32_t t = ts[i];
            p[0] = static_cast<uint8_t>(t); hash ^= static_cast<uint64_t>(t & 0xFF); hash *= FNV_PRIME; ++p;
            p[0] = static_cast<uint8_t>(t >> 8); hash ^= static_cast<uint64_t>((t >> 8) & 0xFF); hash *= FNV_PRIME; ++p;
            p[0] = static_cast<uint8_t>(t >> 16); hash ^= static_cast<uint64_t>((t >> 16) & 0xFF); hash *= FNV_PRIME; ++p;
            p[0] = static_cast<uint8_t>(t >> 24); hash ^= static_cast<uint64_t>((t >> 24) & 0xFF); hash *= FNV_PRIME; ++p;

            uint16_t l = levels[i];
            p[0] = static_cast<uint8_t>(l); hash ^= static_cast<uint64_t>(l & 0xFF); hash *= FNV_PRIME; ++p;
            p[0] = static_cast<uint8_t>(l >> 8); hash ^= static_cast<uint64_t>((l >> 8) & 0xFF); hash *= FNV_PRIME; ++p;

            uint16_t le = lens[i];
            p[0] = static_cast<uint8_t>(le); hash ^= static_cast<uint64_t>(le & 0xFF); hash *= FNV_PRIME; ++p;
            p[0] = static_cast<uint8_t>(le >> 8); hash ^= static_cast<uint64_t>((le >> 8) & 0xFF); hash *= FNV_PRIME; ++p;
        }
    }
    return hash;
}