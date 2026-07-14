#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>

namespace {

inline void fnv1a_update(uint64_t& hash, unsigned char c) {
    hash ^= static_cast<uint64_t>(c);
    hash *= 1099511628211ULL;
}

inline void fnv1a_update_uint(uint64_t& hash, uint32_t val) {
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
        fnv1a_update(hash, static_cast<unsigned char>(buf[j]));
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
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (std::size_t i = 0; i < ids.size(); ++i) {
            fnv1a_update_uint(hash, timestamps[i]);
            fnv1a_update(hash, static_cast<unsigned char>('|'));
            fnv1a_update_uint(hash, ids[i]);
            fnv1a_update(hash, static_cast<unsigned char>('|'));
            fnv1a_update_uint(hash, levels[i]);
            fnv1a_update(hash, static_cast<unsigned char>('|'));
            
            const std::string& msg = messages[i];
            for (char c : msg) {
                fnv1a_update(hash, static_cast<unsigned char>(c));
            }
            fnv1a_update(hash, static_cast<unsigned char>('\n'));
        }
    }
    return hash;
}
