#include "interface.h"
#include <vector>
#include <string>
#include <cstdio>
#include <algorithm>

namespace {
inline uint64_t compute_checksum(const char* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}

inline char* write_uint(char* p, uint32_t val) {
    if (val == 0) { *p++ = '0'; return p; }
    char buf[10];
    int i = 0;
    while (val > 0) { buf[i++] = (val % 10) + '0'; val /= 10; }
    while (i > 0) { *p++ = buf[--i]; }
    return p;
}
} // namespace

uint64_t batched_log_checksum(const std::vector<uint32_t>& timestamps, const std::vector<uint32_t>& ids, const std::vector<uint16_t>& levels, const std::vector<std::string>& messages, int iters) {
    size_t total_size = 0;
    size_t n = ids.size();
    std::vector<size_t> msg_sizes(n);
    for (size_t i = 0; i < n; ++i) {
        msg_sizes[i] = messages[i].size();
        total_size += 30 + msg_sizes[i]; // Max digits + separators + newline
    }

    std::vector<char> buffer(total_size);
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        char* p = buffer.data();
        for (size_t i = 0; i < n; ++i) {
            p = write_uint(p, timestamps[i]);
            *p++ = '|';
            p = write_uint(p, ids[i]);
            *p++ = '|';
            p = write_uint(p, levels[i]);
            *p++ = '|';
            const std::string& m = messages[i];
            std::memcpy(p, m.data(), msg_sizes[i]);
            p += msg_sizes[i];
            *p++ = '\n';
        }
        hash = compute_checksum(buffer.data(), p - buffer.data());
    }
    return hash;
}