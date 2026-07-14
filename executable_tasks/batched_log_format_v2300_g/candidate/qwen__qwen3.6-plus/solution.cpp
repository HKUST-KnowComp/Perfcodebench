#include "interface.h"
#include <vector>
#include <string>

namespace {

inline void hash_uint(uint32_t v, uint64_t& h) {
    char buf[10];
    int idx = 9;
    do {
        buf[idx--] = '0' + (v % 10);
        v /= 10;
    } while (v);
    ++idx;
    while (idx <= 9) {
        h ^= static_cast<uint64_t>(static_cast<unsigned char>(buf[idx]));
        h *= 1099511628211ULL;
        ++idx;
    }
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    uint64_t hash = 0;
    const size_t n = ids.size();
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (size_t i = 0; i < n; ++i) {
            hash_uint(timestamps[i], hash);
            hash ^= '|'; hash *= FNV_PRIME;
            
            hash_uint(ids[i], hash);
            hash ^= '|'; hash *= FNV_PRIME;
            
            hash_uint(levels[i], hash);
            hash ^= '|'; hash *= FNV_PRIME;
            
            for (unsigned char c : messages[i]) {
                hash ^= static_cast<uint64_t>(c);
                hash *= FNV_PRIME;
            }
            hash ^= '\n'; hash *= FNV_PRIME;
        }
    }
    return hash;
}
