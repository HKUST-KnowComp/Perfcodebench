#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace {

constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline void hash_byte(uint64_t& h, unsigned char c) {
    h ^= c;
    h *= FNV_PRIME;
}

inline void hash_uint(uint64_t& h, uint32_t v) {
    char buf[12];
    char* p = buf + 11;
    if (v == 0) {
        *--p = '0';
    } else {
        while (v > 0) {
            *--p = '0' + (v % 10);
            v /= 10;
        }
    }
    while (p < buf + 11) {
        hash_byte(h, static_cast<unsigned char>(*p++));
    }
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    (void)iters;
    uint64_t h = 1469598103934665603ULL;
    const std::size_t n = ids.size();
    for (std::size_t i = 0; i < n; ++i) {
        hash_uint(h, timestamps[i]);
        hash_byte(h, '|');
        hash_uint(h, ids[i]);
        hash_byte(h, '|');
        hash_uint(h, levels[i]);
        hash_byte(h, '|');
        for (unsigned char c : messages[i]) {
            hash_byte(h, c);
        }
        hash_byte(h, '\n');
    }
    return h;
}