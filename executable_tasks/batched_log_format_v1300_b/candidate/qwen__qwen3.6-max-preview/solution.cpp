#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>

namespace {

inline void update_hash(uint64_t& hash, char c) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
    hash *= 1099511628211ULL;
}

inline void hash_uint(uint64_t& hash, uint32_t val) {
    if (val == 0) {
        update_hash(hash, '0');
        return;
    }
    char buf[12];
    int len = 0;
    while (val > 0) {
        buf[len++] = '0' + (val % 10);
        val /= 10;
    }
    for (int i = len - 1; i >= 0; --i) {
        update_hash(hash, buf[i]);
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
        hash = 1469598103934665603ULL;
        for (size_t i = 0; i < n; ++i) {
            hash_uint(hash, timestamps[i]);
            update_hash(hash, '|');
            hash_uint(hash, ids[i]);
            update_hash(hash, '|');
            hash_uint(hash, levels[i]);
            update_hash(hash, '|');
            for (char c : messages[i]) {
                update_hash(hash, c);
            }
            update_hash(hash, '\n');
        }
    }
    return hash;
}
