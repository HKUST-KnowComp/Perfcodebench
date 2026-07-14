#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>

namespace {

constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME  = 1099511628211ULL;

inline void update_hash(uint64_t& h, unsigned char c) {
    h ^= static_cast<uint64_t>(c);
    h *= FNV_PRIME;
}

inline void hash_u32(uint64_t& h, uint32_t v) {
    if (v == 0) {
        update_hash(h, '0');
        return;
    }
    char buf[10];
    int n = 0;
    while (v) {
        buf[n++] = char('0' + (v % 10));
        v /= 10;
    }
    while (n--) {
        update_hash(h, static_cast<unsigned char>(buf[n]));
    }
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    uint64_t hash = 0;
    const size_t n = ids.size();
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (size_t i = 0; i < n; ++i) {
            hash_u32(hash, timestamps[i]);
            update_hash(hash, '|');
            hash_u32(hash, ids[i]);
            update_hash(hash, '|');
            hash_u32(hash, static_cast<uint32_t>(levels[i]));
            update_hash(hash, '|');
            
            const std::string& msg = messages[i];
            const char* mdata = msg.data();
            size_t mlen = msg.size();
            for (size_t j = 0; j < mlen; ++j) {
                update_hash(hash, static_cast<unsigned char>(mdata[j]));
            }
            update_hash(hash, '\n');
        }
    }
    return hash;
}