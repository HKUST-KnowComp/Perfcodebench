#include "interface.h"
#include <cstdio>
#include <vector>
#include <string>
#include <charconv>

namespace {

// FNV-1a 64-bit hash
inline uint64_t checksum_text(const char* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
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
        // Max digits for uint32 is 10, uint16 is 5. Plus separators and newline.
        total_size += 10 + 1 + 10 + 1 + 5 + 1 + msg_sizes[i] + 1;
    }

    std::vector<char> buffer(total_size);
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        char* ptr = buffer.data();
        for (size_t i = 0; i < n; ++i) {
            auto res = std::to_chars(ptr, ptr + 10, timestamps[i]);
            ptr = res.ptr;
            *ptr++ = '|';
            res = std::to_chars(ptr, ptr + 10, ids[i]);
            ptr = res.ptr;
            *ptr++ = '|';
            res = std::to_chars(ptr, ptr + 5, levels[i]);
            ptr = res.ptr;
            *ptr++ = '|';
            const std::string& msg = messages[i];
            memcpy(ptr, msg.data(), msg_sizes[i]);
            ptr += msg_sizes[i];
            *ptr++ = '\n';
        }
        hash = checksum_text(buffer.data(), ptr - buffer.data());
    }
    return hash;
}