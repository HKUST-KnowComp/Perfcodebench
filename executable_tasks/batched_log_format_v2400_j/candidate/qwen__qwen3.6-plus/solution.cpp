#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>

namespace {

inline void fnv1a_update(uint64_t& h, unsigned char c) {
    h ^= static_cast<uint64_t>(c);
    h *= 1099511628211ULL;
}

inline void hash_uint32(uint64_t& h, uint32_t v) {
    if (v == 0) {
        fnv1a_update(h, '0');
        return;
    }
    char buf[10];
    int len = 0;
    do {
        buf[len++] = '0' + (v % 10);
        v /= 10;
    } while (v);
    for (int i = len - 1; i >= 0; --i) {
        fnv1a_update(h, static_cast<unsigned char>(buf[i]));
    }
}

inline void hash_uint16(uint64_t& h, uint16_t v) {
    hash_uint32(h, v);
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    uint64_t result_hash = 0;
    const std::size_t n = ids.size();
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = 1469598103934665603ULL;
        for (std::size_t i = 0; i < n; ++i) {
            hash_uint32(h, timestamps[i]);
            fnv1a_update(h, '|');
            hash_uint32(h, ids[i]);
            fnv1a_update(h, '|');
            hash_uint16(h, levels[i]);
            fnv1a_update(h, '|');
            for (unsigned char c : messages[i]) {
                fnv1a_update(h, c);
            }
            fnv1a_update(h, '\n');
        }
        result_hash = h;
    }
    return result_hash;
}
