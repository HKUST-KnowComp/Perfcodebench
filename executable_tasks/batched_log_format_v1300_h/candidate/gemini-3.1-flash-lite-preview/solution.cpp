#include "interface.h"
#include <string>
#include <vector>
#include <cstdio>

namespace {
inline uint64_t update_hash(uint64_t hash, const char* data, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}

// Fast integer to string conversion into a buffer
inline char* write_uint(char* p, uint32_t val) {
    if (val == 0) { *p++ = '0'; return p; }
    char buf[10];
    int i = 0;
    while (val > 0) { buf[i++] = (val % 10) + '0'; val /= 10; }
    while (--i >= 0) *p++ = buf[i];
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
    std::vector<char> buffer(n * 128); // Heuristic buffer size
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        char* ptr = buffer.data();
        for (size_t i = 0; i < n; ++i) {
            ptr = write_uint(ptr, timestamps[i]);
            *ptr++ = '|';
            ptr = write_uint(ptr, ids[i]);
            *ptr++ = '|';
            ptr = write_uint(ptr, levels[i]);
            *ptr++ = '|';
            const std::string& msg = messages[i];
            memcpy(ptr, msg.data(), msg.size());
            ptr += msg.size();
            *ptr++ = '\n';
        }
        hash = 1469598103934665603ULL;
        hash = update_hash(hash, buffer.data(), ptr - buffer.data());
    }
    return hash;
}