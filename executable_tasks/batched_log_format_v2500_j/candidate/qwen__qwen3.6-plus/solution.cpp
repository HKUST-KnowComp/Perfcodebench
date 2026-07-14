#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>

namespace {

inline void update_hash(uint64_t& hash, uint64_t prime, unsigned char c) {
    hash ^= static_cast<uint64_t>(c);
    hash *= prime;
}

inline void append_uint_hash(uint64_t& hash, uint64_t prime, uint32_t v) {
    if (v == 0) {
        update_hash(hash, prime, '0');
        return;
    }
    char buf[12];
    int n = 0;
    do {
        buf[n++] = '0' + (v % 10);
        v /= 10;
    } while (v);
    while (n--) {
        update_hash(hash, prime, static_cast<unsigned char>(buf[n]));
    }
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (size_t i = 0; i < ids.size(); ++i) {
            append_uint_hash(hash, FNV_PRIME, timestamps[i]);
            update_hash(hash, FNV_PRIME, '|');
            
            append_uint_hash(hash, FNV_PRIME, ids[i]);
            update_hash(hash, FNV_PRIME, '|');
            
            append_uint_hash(hash, FNV_PRIME, levels[i]);
            update_hash(hash, FNV_PRIME, '|');
            
            for (unsigned char c : messages[i]) {
                update_hash(hash, FNV_PRIME, c);
            }
            update_hash(hash, FNV_PRIME, '\n');
        }
    }
    return hash;
}
