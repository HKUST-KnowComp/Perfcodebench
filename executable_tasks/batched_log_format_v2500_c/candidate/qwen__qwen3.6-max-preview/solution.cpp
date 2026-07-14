#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

inline void hash_update(uint64_t& h, unsigned char c) {
    h ^= static_cast<uint64_t>(c);
    h *= 1099511628211ULL;
}

inline void hash_update_uint(uint64_t& h, uint32_t val) {
    char buf[12];
    int len = 0;
    if (val == 0) {
        buf[len++] = '0';
    } else {
        while (val > 0) {
            buf[len++] = '0' + (val % 10);
            val /= 10;
        }
    }
    for (int j = len - 1; j >= 0; --j) {
        hash_update(h, static_cast<unsigned char>(buf[j]));
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
    size_t n = ids.size();
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = 1469598103934665603ULL;
        for (size_t i = 0; i < n; ++i) {
            hash_update_uint(h, timestamps[i]);
            hash_update(h, '|');
            hash_update_uint(h, ids[i]);
            hash_update(h, '|');
            hash_update_uint(h, levels[i]);
            hash_update(h, '|');
            
            for (unsigned char c : messages[i]) {
                hash_update(h, c);
            }
            hash_update(h, '\n');
        }
        final_hash = h;
    }
    return final_hash;
}
