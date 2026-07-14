#include "interface.h"
#include <vector>
#include <string>
#include <cstdio>
#include <algorithm>

namespace {
inline uint64_t update_checksum(uint64_t hash, const char* data, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}

// Fast integer to string conversion into a pre-allocated buffer
char* write_uint(char* p, uint32_t val) {
    if (val == 0) { *p++ = '0'; return p; }
    char buf[10];
    int i = 0;
    while (val > 0) { buf[i++] = (val % 10) + '0'; val /= 10; }
    while (i > 0) { *p++ = buf[--i]; }
    return p;
}
} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    size_t n = ids.size();
    std::vector<size_t> msg_sizes(n);
    size_t total_size = 0;
    for (size_t i = 0; i < n; ++i) {
        msg_sizes[i] = messages[i].size();
        // Max digits for uint32 is 10, uint16 is 5
        total_size += 10 + 1 + 10 + 1 + 5 + 1 + msg_sizes[i] + 1;
    }

    std::vector<char> buffer(total_size);
    uint64_t final_hash = 0;

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
        final_hash = 1469598103934665603ULL;
        final_hash = update_checksum(final_hash, buffer.data(), p - buffer.data());
    }
    return final_hash;
}