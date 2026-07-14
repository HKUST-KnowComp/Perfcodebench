#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

inline void update_hash(uint64_t& h, unsigned char c) {
    h ^= c;
    h *= 1099511628211ULL;
}

inline void hash_u32(uint64_t& h, uint32_t v) {
    if (v == 0) {
        update_hash(h, '0');
        return;
    }
    char buf[10];
    int n = 0;
    while (v > 0) {
        buf[n++] = '0' + (v % 10);
        v /= 10;
    }
    for (int i = n - 1; i >= 0; --i) {
        update_hash(h, static_cast<unsigned char>(buf[i]));
    }
}

inline void hash_str(uint64_t& h, const std::string& s) {
    for (unsigned char c : s) {
        update_hash(h, c);
    }
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    uint64_t final_hash = 0;
    const size_t n = ids.size();
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = 1469598103934665603ULL;
        for (size_t i = 0; i < n; ++i) {
            hash_u32(h, timestamps[i]);
            update_hash(h, static_cast<unsigned char>('|'));
            hash_u32(h, ids[i]);
            update_hash(h, static_cast<unsigned char>('|'));
            hash_u32(h, levels[i]);
            update_hash(h, static_cast<unsigned char>('|'));
            hash_str(h, messages[i]);
            update_hash(h, static_cast<unsigned char>('\n'));
        }
        final_hash = h;
    }
    return final_hash;
}
