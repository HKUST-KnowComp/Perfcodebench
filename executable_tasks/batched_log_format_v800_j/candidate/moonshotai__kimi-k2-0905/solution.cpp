#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>
#include <cstring>

namespace {

// Fast integer-to-string for 32-bit unsigned, returns pointer past last digit
inline char* u32toa(uint32_t value, char* buffer) noexcept {
    char tmp[16];
    char* p = tmp;
    do {
        *p++ = char('0' + (value % 10));
        value /= 10;
    } while (value);
    char* out = buffer;
    while (p != tmp) *out++ = *--p;
    return out;
}

// Fast integer-to-string for 16-bit unsigned
inline char* u16toa(uint16_t value, char* buffer) noexcept {
    return u32toa(value, buffer);
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {

    const size_t n = ids.size();
    // Pre-compute a tight upper bound for one iteration
    size_t cap = 0;
    for (size_t i = 0; i < n; ++i) {
        // 20 digits max per number + 3 pipes + newline + message
        cap += 20 + 1 + 20 + 1 + 5 + 1 + messages[i].size() + 1;
    }

    std::string buf;
    buf.reserve(cap);

    uint64_t hash = 1469598103934665603ULL;
    for (int iter = 0; iter < iters; ++iter) {
        buf.clear();
        hash = 1469598103934665603ULL; // reset hash each iteration

        for (size_t i = 0; i < n; ++i) {
            // Format directly into buffer
            char* base = &buf[buf.size()];
            char* p = base;

            p = u32toa(timestamps[i], p);
            *p++ = '|';
            p = u32toa(ids[i], p);
            *p++ = '|';
            p = u16toa(levels[i], p);
            *p++ = '|';

            const std::string& msg = messages[i];
            const size_t msgLen = msg.size();
            buf.resize(buf.size() + (p - base) + msgLen + 1);
            // copy message + newline
            std::memcpy(p, msg.data(), msgLen);
            p[msgLen] = '\n';

            // update rolling checksum while we have the bytes hot in cache
            const unsigned char* data = reinterpret_cast<const unsigned char*>(base);
            const size_t total = (p - base) + msgLen + 1;
            for (size_t k = 0; k < total; ++k) {
                hash ^= static_cast<uint64_t>(data[k]);
                hash *= 1099511628211ULL;
            }
        }
    }
    return hash;
}