#include "interface.h"
#include <string>
#include <vector>
#include <cstdint>
#include <cstring>

namespace {

// FNV-1a checksum
inline uint64_t checksum_add(uint64_t h, const void* data, std::size_t len) {
    const unsigned char* p = static_cast<const unsigned char*>(data);
    const uint64_t prime = 1099511628211ULL;
    for (std::size_t i = 0; i < len; ++i) {
        h ^= p[i];
        h *= prime;
    }
    return h;
}

// Fast uint32 -> decimal, returns pointer to one-past-last-char
inline char* u32toa(uint32_t value, char* buffer) {
    char tmp[16];
    char* end = tmp + 16;
    char* p = end;
    do {
        *--p = char('0' + (value % 10));
        value /= 10;
    } while (value);
    std::size_t len = end - p;
    std::memcpy(buffer, p, len);
    return buffer + len;
}

// Fast uint16 -> decimal
inline char* u16toa(uint16_t value, char* buffer) {
    return u32toa(value, buffer);
}

} // anonymous namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters)
{
    const std::size_t n = ids.size();
    // Pre-compute total size once
    std::size_t total = 0;
    for (std::size_t i = 0; i < n; ++i) {
        // 20 digits per number + 3 pipes + newline + message
        total += 20 + 1 + 20 + 1 + 5 + 1 + messages[i].size() + 1;
    }

    std::string out;
    out.reserve(total);

    uint64_t hash = 1469598103934665603ULL;
    for (int iter = 0; iter < iters; ++iter) {
        out.clear();
        for (std::size_t i = 0; i < n; ++i) {
            char buf[64];
            char* p = buf;
            p = u32toa(timestamps[i], p);
            *p++ = '|';
            p = u32toa(ids[i], p);
            *p++ = '|';
            p = u16toa(levels[i], p);
            *p++ = '|';
            std::size_t prefix = p - buf;
            std::size_t msglen = messages[i].size();
            out.append(buf, prefix);
            out.append(messages[i]);
            out.push_back('\n');
        }
        hash = checksum_add(hash, out.data(), out.size());
    }
    return hash;
}