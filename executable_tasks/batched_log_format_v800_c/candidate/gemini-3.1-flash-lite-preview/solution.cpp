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

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    size_t total_size = 0;
    size_t n = ids.size();
    std::vector<size_t> msg_sizes(n);
    for (size_t i = 0; i < n; ++i) {
        msg_sizes[i] = messages[i].size();
        // Max digits for uint32 is 10, uint16 is 5
        total_size += 10 + 1 + 10 + 1 + 5 + 1 + msg_sizes[i] + 1;
    }

    std::vector<char> buffer(total_size);
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        char* ptr = buffer.data();
        uint64_t hash = 1469598103934665603ULL;
        
        for (size_t i = 0; i < n; ++i) {
            char* start = ptr;
            ptr = write_uint(ptr, timestamps[i]);
            *ptr++ = '|';
            ptr = write_uint(ptr, ids[i]);
            *ptr++ = '|';
            ptr = write_uint(ptr, levels[i]);
            *ptr++ = '|';
            const std::string& msg = messages[i];
            memcpy(ptr, msg.data(), msg_sizes[i]);
            ptr += msg_sizes[i];
            *ptr++ = '\n';
            
            hash = update_hash(hash, start, ptr - start);
        }
        final_hash = hash;
    }
    return final_hash;
}