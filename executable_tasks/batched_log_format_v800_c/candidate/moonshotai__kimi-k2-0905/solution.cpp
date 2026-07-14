#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>
#include <cstring>

namespace {

inline uint32_t u32toa(uint32_t value, char* buf) {
    char tmp[16];
    char* p = tmp;
    do {
        *p++ = char('0' + (value % 10));
        value /= 10;
    } while (value);
    uint32_t len = p - tmp;
    uint32_t ans = len;
    while (p > tmp) *buf++ = *--p;
    return ans;
}

inline uint64_t checksum_incremental(uint64_t hash, const char* data, size_t len) {
    const uint64_t prime = 1099511628211ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= prime;
    }
    return hash;
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {

    const size_t n = ids.size();
    std::string out;
    out.reserve(64 * n);

    uint64_t hash = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        out.clear();
        hash = 1469598103934665603ULL;

        for (size_t i = 0; i < n; ++i) {
            char buf[64];
            char* p = buf;
            p += u32toa(timestamps[i], p);
            *p++ = '|';
            p += u32toa(ids[i], p);
            *p++ = '|';
            p += u32toa(levels[i], p);
            *p++ = '|';
            const std::string& msg = messages[i];
            memcpy(p, msg.data(), msg.size());
            p += msg.size();
            *p++ = '\n';
            size_t len = p - buf;
            out.append(buf, len);
            hash = checksum_incremental(hash, buf, len);
        }
    }
    return hash;
}