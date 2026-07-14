#include "interface.h"
#include <string>
#include <vector>
#include <cstdio>
#include <algorithm>

namespace {

// Fast integer to string conversion into a pre-allocated buffer
char* write_uint(char* p, uint32_t val) {
    if (val == 0) { *p++ = '0'; return p; }
    char buf[12];
    int i = 0;
    while (val > 0) {
        buf[i++] = (val % 10) + '0';
        val /= 10;
    }
    while (i > 0) *p++ = buf[--i];
    return p;
}

uint64_t checksum_text(const char* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= 1099511628211ULL;
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
    
    size_t total_size = 0;
    for (size_t i = 0; i < ids.size(); ++i) {
        total_size += 32 + messages[i].size(); // Conservative estimate
    }

    std::vector<char> buffer(total_size * 2);
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        char* p = buffer.data();
        for (size_t i = 0; i < ids.size(); ++i) {
            p = write_uint(p, timestamps[i]);
            *p++ = '|';
            p = write_uint(p, ids[i]);
            *p++ = '|';
            p = write_uint(p, levels[i]);
            *p++ = '|';
            const std::string& msg = messages[i];
            memcpy(p, msg.data(), msg.size());
            p += msg.size();
            *p++ = '\n';
        }
        hash = checksum_text(buffer.data(), p - buffer.data());
    }
    return hash;
}