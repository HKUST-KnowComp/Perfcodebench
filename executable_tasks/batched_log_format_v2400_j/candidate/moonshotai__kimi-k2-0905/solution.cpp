#include "interface.h"

#include <string>
#include <vector>
#include <cstdio>
#include <cstring>

namespace {

inline uint64_t checksum_update(uint64_t hash, const char* data, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        unsigned char c = data[i];
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

    const size_t n = ids.size();

    // Pre-compute the exact size needed for one full iteration
    size_t total = 0;
    for (size_t i = 0; i < n; ++i) {
        // 20 digits max per uint32, 5 per uint16, 3 pipes, 1 newline, message length
        total += 20 + 1 + 20 + 1 + 5 + 1 + messages[i].size() + 1;
    }

    std::string buffer;
    buffer.reserve(total);

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        buffer.clear();
        char tmp[64];
        for (size_t i = 0; i < n; ++i) {
            int len = std::snprintf(tmp, sizeof(tmp), "%u|%u|%u|",
                                    timestamps[i], ids[i], levels[i]);
            buffer.append(tmp, len);
            buffer.append(messages[i]);
            buffer.push_back('\n');
        }
        hash = checksum_update(1469598103934665603ULL, buffer.data(), buffer.size());
    }
    return hash;
}