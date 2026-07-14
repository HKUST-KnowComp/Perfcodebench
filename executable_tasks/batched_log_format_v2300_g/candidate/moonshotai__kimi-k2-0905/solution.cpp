#include "interface.h"

#include <string>
#include <vector>
#include <cstring>

namespace {

inline uint64_t checksum_text(const char* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

inline uint32_t u32toa_len(uint32_t x) {
    if (x < 10) return 1;
    if (x < 100) return 2;
    if (x < 1000) return 3;
    if (x < 10000) return 4;
    if (x < 100000) return 5;
    if (x < 1000000) return 6;
    if (x < 10000000) return 7;
    if (x < 100000000) return 8;
    if (x < 1000000000) return 9;
    return 10;
}
inline uint32_t u16toa_len(uint16_t x) {
    if (x < 10) return 1;
    if (x < 100) return 2;
    if (x < 1000) return 3;
    if (x < 10000) return 4;
    return 5;
}

inline char* u32toa(char* buf, uint32_t x) {
    char tmp[16];
    char* p = tmp;
    do { *p++ = char('0' + (x % 10)); x /= 10; } while (x);
    char* out = buf;
    while (p != tmp) *out++ = *--p;
    return out;
}
inline char* u16toa(char* buf, uint16_t x) {
    char tmp[8];
    char* p = tmp;
    do { *p++ = char('0' + (x % 10)); x /= 10; } while (x);
    char* out = buf;
    while (p != tmp) *out++ = *--p;
    return out;
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    const size_t n = ids.size();
    size_t total = 0;
    for (size_t i = 0; i < n; ++i) {
        total += u32toa_len(timestamps[i]) + 1 +
                 u32toa_len(ids[i]) + 1 +
                 u16toa_len(levels[i]) + 1 +
                 messages[i].size() + 1;
    }
    std::string buffer;
    buffer.reserve(total);
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        buffer.clear();
        char* ptr = &buffer[0];
        for (size_t i = 0; i < n; ++i) {
            ptr = u32toa(ptr, timestamps[i]); *ptr++ = '|';
            ptr = u32toa(ptr, ids[i]);       *ptr++ = '|';
            ptr = u16toa(ptr, levels[i]);     *ptr++ = '|';
            const std::string& msg = messages[i];
            memcpy(ptr, msg.data(), msg.size());
            ptr += msg.size();
            *ptr++ = '\n';
        }
        buffer.resize(ptr - &buffer[0]);
    }
    hash = checksum_text(buffer.data(), buffer.size());
    return hash;
}