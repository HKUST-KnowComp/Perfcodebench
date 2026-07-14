#include "interface.h"
#include <vector>
#include <string>
#include <cstdint>

namespace {

inline void feed_char(uint64_t& h, char c) {
    h ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
    h *= 1099511628211ULL;
}

inline void feed_uint32(uint64_t& h, uint32_t v) {
    if (v == 0) {
        feed_char(h, '0');
        return;
    }
    char buf[10];
    int p = 10;
    while (v > 0) {
        buf[--p] = '0' + (v % 10);
        v /= 10;
    }
    for (int i = p; i < 10; ++i) {
        feed_char(h, buf[i]);
    }
}

inline void feed_uint16(uint64_t& h, uint16_t v) {
    if (v == 0) {
        feed_char(h, '0');
        return;
    }
    char buf[5];
    int p = 5;
    while (v > 0) {
        buf[--p] = '0' + (v % 10);
        v /= 10;
    }
    for (int i = p; i < 5; ++i) {
        feed_char(h, buf[i]);
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
    const uint32_t* ts_ptr = timestamps.data();
    const uint32_t* id_ptr = ids.data();
    const uint16_t* lv_ptr = levels.data();
    const std::string* msg_ptr = messages.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = 1469598103934665603ULL;
        for (size_t i = 0; i < n; ++i) {
            feed_uint32(h, ts_ptr[i]);
            feed_char(h, '|');
            feed_uint32(h, id_ptr[i]);
            feed_char(h, '|');
            feed_uint16(h, lv_ptr[i]);
            feed_char(h, '|');
            for (char c : msg_ptr[i]) {
                feed_char(h, c);
            }
            feed_char(h, '\n');
        }
        hash = h;
    }
    return hash;
}
