#include "interface.h"

#include <cstring>
#include <string>
#include <vector>

namespace {

inline uint64_t checksum_text(const char* first, const char* last) {
    uint64_t hash = 1469598103934665603ULL;
    while (first != last) {
        unsigned char c = *first++;
        hash ^= static_cast<uint64_t>(c);
        hash *= 1099511628211ULL;
    }
    return hash;
}

// Fast integer → decimal, returns pointer past last digit.
inline char* u32_to_str(char* buf, uint32_t v) {
    if (v == 0) { *buf++ = '0'; return buf; }
    char tmp[10];                    // 4294967295 has 10 digits
    char* p = tmp + 10;
    while (v) {
        *--p = char('0' + (v % 10));
        v /= 10;
    }
    std::size_t len = tmp + 10 - p;
    std::memcpy(buf, p, len);
    return buf + len;
}

inline char* u16_to_str(char* buf, uint16_t v) {
    return u32_to_str(buf, v);
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {

    const std::size_t n = ids.size();

    // Pre-compute the total byte count of the final output.
    std::size_t total = 0;
    for (std::size_t i = 0; i < n; ++i) {
        // ts|id|level|msg\n
        // 10 digits + 1 + 10 digits + 1 + 5 digits + 1 + len + 1
        total += 10 + 1 + 10 + 1 + 5 + 1 + messages[i].size() + 1;
    }

    // Allocate once, reuse every iteration.
    std::string buffer;
    buffer.resize(total);

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        char* dst = &buffer[0];
        for (std::size_t i = 0; i < n; ++i) {
            dst = u32_to_str(dst, timestamps[i]);
            *dst++ = '|';
            dst = u32_to_str(dst, ids[i]);
            *dst++ = '|';
            dst = u16_to_str(dst, levels[i]);
            *dst++ = '|';
            std::memcpy(dst, messages[i].data(), messages[i].size());
            dst += messages[i].size();
            *dst++ = '\n';
        }
        hash = checksum_text(buffer.data(), dst);
    }
    return hash;
}