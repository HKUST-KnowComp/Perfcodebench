#include "interface.h"
#include <string>
#include <vector>
#include <cstdio>
#include <algorithm>

namespace {

// Fast integer to string conversion into a pre-allocated buffer
inline char* write_uint(char* p, uint32_t val) {
    if (val == 0) { *p++ = '0'; return p; }
    char buf[10];
    int i = 0;
    while (val > 0) { buf[i++] = (val % 10) + '0'; val /= 10; }
    while (i > 0) { *p++ = buf[--i]; }
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
    
    size_t n = ids.size();
    std::vector<std::string> lines(n);
    size_t total_size = 0;
    
    // Pre-format lines once
    for (size_t i = 0; i < n; ++i) {
        lines[i] = std::to_string(timestamps[i]) + "|" + 
                   std::to_string(ids[i]) + "|" + 
                   std::to_string(levels[i]) + "|" + 
                   messages[i] + "\n";
        total_size += lines[i].size();
    }

    std::string out;
    out.reserve(total_size);
    for (const auto& line : lines) {
        out.append(line);
    }

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = checksum_text(out);
    }
    return hash;
}