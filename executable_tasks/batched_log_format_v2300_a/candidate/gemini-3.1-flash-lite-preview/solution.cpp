#include "interface.h"
#include <string>
#include <vector>
#include <cstdio>
#include <algorithm>

namespace {

// Fast integer to string conversion into a pre-allocated buffer
char* write_uint(char* p, uint32_t val) {
    if (val == 0) { *p++ = '0'; return p; }
    char buf[10];
    int i = 0;
    while (val > 0) {
        buf[i++] = (val % 10) + '0';
        val /= 10;
    }
    while (i > 0) *p++ = buf[--i];
    return p;
}

uint64_t checksum_text(const std::string& text) {
    uint64_t hash = 1469598103934665603ULL;
    for (unsigned char c : text) {
        hash ^= static_cast<uint64_t>(c);
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

    std::string out;
    out.reserve(total_size);
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        out.clear();
        for (size_t i = 0; i < ids.size(); ++i) {
            char buf[32];
            char* p = buf;
            p = write_uint(p, timestamps[i]);
            *p++ = '|';
            p = write_uint(p, ids[i]);
            *p++ = '|';
            p = write_uint(p, levels[i]);
            *p++ = '|';
            out.append(buf, p - buf);
            out.append(messages[i]);
            out.push_back('\n');
        }
        hash = checksum_text(out);
    }
    return hash;
}