#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>
#include <cstddef>

namespace {

inline void hash_update(uint64_t& h, char c) {
    h ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
    h *= 1099511628211ULL;
}

inline void hash_append_uint(uint64_t& h, uint32_t val) {
    char buf[12];
    char* p = buf + 12;
    if (val == 0) {
        *--p = '0';
    } else {
        while (val > 0) {
            *--p = '0' + (val % 10);
            val /= 10;
        }
    }
    while (p < buf + 12) {
        hash_update(h, *p++);
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
            hash_append_uint(h, timestamps[i]);
            hash_update(h, '|');
            hash_append_uint(h, ids[i]);
            hash_update(h, '|');
            hash_append_uint(h, levels[i]);
            hash_update(h, '|');
            
            const std::string& msg = messages[i];
            const char* mdata = msg.data();
            size_t mlen = msg.size();
            for (size_t j = 0; j < mlen; ++j) {
                hash_update(h, mdata[j]);
            }
            hash_update(h, '\n');
        }
        final_hash = h;
    }
    return final_hash;
}