#include "interface.h"
#include <cstring>
#include <cstdint>
#include <vector>

namespace {

inline uint64_t checksum(const char* first, const char* last) {
    uint64_t h = 1469598103934665603ULL;
    while (first != last) {
        h ^= static_cast<uint64_t>(static_cast<unsigned char>(*first++));
        h *= 1099511628211ULL;
    }
    return h;
}

// write int into buf and return new pointer
inline char* write_u32(char* p, uint32_t v) {
    // digits
    if (v < 10) { *p++ = '0' + v; }
    else {
        char tmp[16];
        char* t = tmp + 16;
        do {
            *--t = '0' + (v % 10); v /= 10;
        } while (v);
        size_t len = (tmp + 16) - t;
        std::memcpy(p, t, len);
        p += len;
    }
    return p;
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {

    size_t total_bytes = 0;
    for (size_t i = 0; i < ids.size(); ++i) {
        // timestamp + '|' + id + '|' + level + '|' + message + '
'
        // pessimistically estimate digits (up to 10 per number)
        total_bytes += 10 + 1 + 10 + 1 + 5 + 1 + messages[i].size() + 1;
    }
    std::vector<char> buffer;
    buffer.resize(total_bytes + 1);  // reserve space

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        char* start = &buffer[0];
        char* pos   = start;
        for (size_t i = 0; i < ids.size(); ++i) {
            pos = write_u32(pos, timestamps[i]);
            *pos++ = '|';
            pos = write_u32(pos, ids[i]);
            *pos++ = '|';
            pos = write_u32(pos, levels[i]);
            *pos++ = '|';

            const std::string& m = messages[i];
            std::memcpy(pos, m.data(), m.size());
            pos += m.size();
            *pos++ = '\n';
        }
        hash = checksum(start, pos);
    }
    return hash;
}