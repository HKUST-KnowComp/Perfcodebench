#include "interface.h"
#include <vector>
#include <string>
#include <cstdio>
#include <algorithm>

namespace {
inline uint64_t update_hash(uint64_t hash, const char* data, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}

// Fast integer to string conversion into a pre-allocated buffer
inline char* write_uint(char* ptr, uint32_t val) {
    if (val == 0) { *ptr++ = '0'; return ptr; }
    char buf[10];
    int i = 0;
    while (val > 0) { buf[i++] = (val % 10) + '0'; val /= 10; }
    while (i > 0) { *ptr++ = buf[--i]; }
    return ptr;
}
} // namespace

uint64_t batched_log_checksum(const std::vector<uint32_t>& timestamps, const std::vector<uint32_t>& ids, const std::vector<uint16_t>& levels, const std::vector<std::string>& messages, int iters) {
    size_t n = ids.size();
    std::vector<size_t> msg_lens(n);
    for (size_t i = 0; i < n; ++i) msg_lens[i] = messages[i].size();

    uint64_t hash = 0;
    std::vector<char> buffer(1024 * 1024); // Initial buffer

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (size_t i = 0; i < n; ++i) {
            char line_buf[64];
            char* p = line_buf;
            p = write_uint(p, timestamps[i]);
            *p++ = '|';
            p = write_uint(p, ids[i]);
            *p++ = '|';
            p = write_uint(p, levels[i]);
            *p++ = '|';
            
            hash = update_hash(hash, line_buf, p - line_buf);
            hash = update_hash(hash, messages[i].data(), msg_lens[i]);
            hash = update_hash(hash, "\n", 1);
        }
    }
    return hash;
}