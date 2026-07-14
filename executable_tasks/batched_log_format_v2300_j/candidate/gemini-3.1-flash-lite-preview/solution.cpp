#include "interface.h"
#include <string>
#include <vector>
#include <cstdio>
#include <algorithm>

namespace {

// Fast integer to string conversion into a buffer
inline char* write_uint(char* p, uint32_t val) {
    if (val == 0) { *p++ = '0'; return p; }
    char buf[10];
    int i = 0;
    while (val > 0) {
        buf[i++] = (val % 10) + '0';
        val /= 10;
    }
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
    std::vector<std::string> formatted_lines(n);
    size_t total_size = 0;

    // Pre-format lines to avoid repeated work
    for (size_t i = 0; i < n; ++i) {
        formatted_lines[i] = std::to_string(timestamps[i]) + "|" + 
                             std::to_string(ids[i]) + "|" + 
                             std::to_string(levels[i]) + "|" + 
                             messages[i] + "\n";
        total_size += formatted_lines[i].size();
    }

    std::vector<char> buffer(total_size);
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        char* ptr = buffer.data();
        for (const auto& line : formatted_lines) {
            std::memcpy(ptr, line.data(), line.size());
            ptr += line.size();
        }

        // FNV-1a checksum
        uint64_t h = 1469598103934665603ULL;
        for (size_t i = 0; i < total_size; ++i) {
            h ^= static_cast<uint64_t>(static_cast<unsigned char>(buffer[i]));
            h *= 1099511628211ULL;
        }
        hash = h;
    }

    return hash;
}