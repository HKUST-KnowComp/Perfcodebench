#include "interface.h"

#include <string>
#include <vector>

namespace {

constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline void append_uint(uint64_t& h, uint32_t val) {
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
    for (int i = len - 1; i >= 0; --i) {
        h ^= static_cast<uint64_t>(static_cast<unsigned char>(buf[i]));
        h *= FNV_PRIME;
    }
}

inline void append_char(uint64_t& h, char c) {
    h ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
    h *= FNV_PRIME;
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    const size_t n = ids.size();
    uint64_t hash = FNV_OFFSET;
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (size_t i = 0; i < n; ++i) {
            append_uint(hash, timestamps[i]);
            append_char(hash, '|');
            append_uint(hash, ids[i]);
            append_char(hash, '|');
            append_uint(hash, levels[i]);
            append_char(hash, '|');
            
            const std::string& msg = messages[i];
            const char* mdata = msg.data();
            size_t mlen = msg.size();
            for (size_t k = 0; k < mlen; ++k) {
                hash ^= static_cast<uint64_t>(static_cast<unsigned char>(mdata[k]));
                hash *= FNV_PRIME;
            }
            
            append_char(hash, '\n');
        }
    }
    return hash;
}
