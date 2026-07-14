#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

inline void fnv_update(uint64_t& h, unsigned char c) {
    h ^= static_cast<uint64_t>(c);
    h *= 1099511628211ULL;
}

inline void fnv_update_uint32(uint64_t& h, uint32_t v) {
    if (v == 0) {
        fnv_update(h, '0');
        return;
    }
    char buf[10];
    int len = 0;
    while (v > 0) {
        buf[len++] = '0' + (v % 10);
        v /= 10;
    }
    for (int i = len - 1; i >= 0; --i) {
        fnv_update(h, static_cast<unsigned char>(buf[i]));
    }
}

inline void fnv_update_uint16(uint64_t& h, uint16_t v) {
    if (v == 0) {
        fnv_update(h, '0');
        return;
    }
    char buf[5];
    int len = 0;
    while (v > 0) {
        buf[len++] = '0' + (v % 10);
        v /= 10;
    }
    for (int i = len - 1; i >= 0; --i) {
        fnv_update(h, static_cast<unsigned char>(buf[i]));
    }
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    uint64_t result_hash = 0;
    const size_t n = ids.size();
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = 1469598103934665603ULL;
        for (size_t i = 0; i < n; ++i) {
            fnv_update_uint32(h, timestamps[i]);
            fnv_update(h, '|');
            fnv_update_uint32(h, ids[i]);
            fnv_update(h, '|');
            fnv_update_uint16(h, levels[i]);
            fnv_update(h, '|');
            const std::string& msg = messages[i];
            for (char c : msg) {
                fnv_update(h, static_cast<unsigned char>(c));
            }
            fnv_update(h, '\n');
        }
        result_hash = h;
    }
    return result_hash;
}
