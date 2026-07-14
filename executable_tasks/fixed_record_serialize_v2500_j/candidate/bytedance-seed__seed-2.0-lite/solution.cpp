#include "interface.h"
#include <vector>
#include <cstdint>

namespace {
constexpr uint64_t FNV1A_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV1A_PRIME = 1099511628211ULL;
}

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
    const size_t num_records = ids.size();
    const size_t record_size = 12; // 4+4+2+2 bytes per fixed record
    std::vector<uint8_t> out;
    out.reserve(num_records * record_size); // Single upfront allocation
    uint8_t* __restrict__ dst = out.data();
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        dst = out.data();
        uint64_t hash = FNV1A_OFFSET;

        for (size_t i = 0; i < num_records; ++i) {
            // Serialize and hash 32-bit ID
            uint32_t v = ids[i];
            uint8_t b;
            b = static_cast<uint8_t>(v); *dst++ = b; hash ^= b; hash *= FNV1A_PRIME;
            b = static_cast<uint8_t>(v >> 8); *dst++ = b; hash ^= b; hash *= FNV1A_PRIME;
            b = static_cast<uint8_t>(v >> 16); *dst++ = b; hash ^= b; hash *= FNV1A_PRIME;
            b = static_cast<uint8_t>(v >> 24); *dst++ = b; hash ^= b; hash *= FNV1A_PRIME;

            // Serialize and hash 32-bit timestamp
            v = ts[i];
            b = static_cast<uint8_t>(v); *dst++ = b; hash ^= b; hash *= FNV1A_PRIME;
            b = static_cast<uint8_t>(v >> 8); *dst++ = b; hash ^= b; hash *= FNV1A_PRIME;
            b = static_cast<uint8_t>(v >> 16); *dst++ = b; hash ^= b; hash *= FNV1A_PRIME;
            b = static_cast<uint8_t>(v >> 24); *dst++ = b; hash ^= b; hash *= FNV1A_PRIME;

            // Serialize and hash 16-bit level
            uint16_t v16 = levels[i];
            b = static_cast<uint8_t>(v16); *dst++ = b; hash ^= b; hash *= FNV1A_PRIME;
            b = static_cast<uint8_t>(v16 >> 8); *dst++ = b; hash ^= b; hash *= FNV1A_PRIME;

            // Serialize and hash 16-bit length
            v16 = lens[i];
            b = static_cast<uint8_t>(v16); *dst++ = b; hash ^= b; hash *= FNV1A_PRIME;
            b = static_cast<uint8_t>(v16 >> 8); *dst++ = b; hash ^= b; hash *= FNV1A_PRIME;
        }
        final_hash = hash;
    }
    return final_hash;
}