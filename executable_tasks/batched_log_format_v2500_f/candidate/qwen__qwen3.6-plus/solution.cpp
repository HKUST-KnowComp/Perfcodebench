#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>

namespace {
constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline void fnv1a_update(uint64_t& h, unsigned char c) {
    h ^= static_cast<uint64_t>(c);
    h *= FNV_PRIME;
}

inline void fnv1a_format_uint32(uint64_t& h, uint32_t v) {
    if (v == 0) {
        fnv1a_update(h, '0');
        return;
    }
    char buf[12];
    char* p = buf + 12;
    while (v > 0) {
        *--p = '0' + (v % 10);
        v /= 10;
    }
    for (char* q = p; q < buf + 12; ++q) {
        fnv1a_update(h, static_cast<unsigned char>(*q));
    }
}
} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    if (iters == 0) return 0;

    uint64_t hash = FNV_OFFSET;
    const size_t n = timestamps.size();
    for (size_t i = 0; i < n; ++i) {
        fnv1a_format_uint32(hash, timestamps[i]);
        fnv1a_update(hash, '|');
        fnv1a_format_uint32(hash, ids[i]);
        fnv1a_update(hash, '|');
        fnv1a_format_uint32(hash, levels[i]);
        fnv1a_update(hash, '|');
        for (unsigned char c : messages[i]) {
            fnv1a_update(hash, c);
        }
        fnv1a_update(hash, '\n');
    }
    return hash;
}