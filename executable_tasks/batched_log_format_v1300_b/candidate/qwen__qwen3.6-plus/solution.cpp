#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>

namespace {

inline int to_str(uint32_t val, char* buf) {
    if (val == 0) { buf[0] = '0'; return 1; }
    int len = 0;
    while (val) {
        buf[len++] = '0' + (val % 10);
        val /= 10;
    }
    for (int i = 0; i < len / 2; ++i) {
        char t = buf[i];
        buf[i] = buf[len - 1 - i];
        buf[len - 1 - i] = t;
    }
    return len;
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
    char num_buf[12];
    size_t n = timestamps.size();
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (size_t i = 0; i < n; ++i) {
            int l1 = to_str(timestamps[i], num_buf);
            for (int k = 0; k < l1; ++k) {
                hash ^= static_cast<uint64_t>(static_cast<unsigned char>(num_buf[k]));
                hash *= FNV_PRIME;
            }
            
            hash ^= '|'; hash *= FNV_PRIME;
            
            int l2 = to_str(ids[i], num_buf);
            for (int k = 0; k < l2; ++k) {
                hash ^= static_cast<uint64_t>(static_cast<unsigned char>(num_buf[k]));
                hash *= FNV_PRIME;
            }
            
            hash ^= '|'; hash *= FNV_PRIME;
            
            int l3 = to_str(levels[i], num_buf);
            for (int k = 0; k < l3; ++k) {
                hash ^= static_cast<uint64_t>(static_cast<unsigned char>(num_buf[k]));
                hash *= FNV_PRIME;
            }
            
            hash ^= '|'; hash *= FNV_PRIME;
            
            const std::string& msg = messages[i];
            const char* mdata = msg.data();
            size_t mlen = msg.size();
            for (size_t k = 0; k < mlen; ++k) {
                hash ^= static_cast<uint64_t>(static_cast<unsigned char>(mdata[k]));
                hash *= FNV_PRIME;
            }
            
            hash ^= '\n'; hash *= FNV_PRIME;
        }
    }
    return hash;
}
