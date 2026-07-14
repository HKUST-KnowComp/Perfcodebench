#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace {

inline uint64_t checksum_text(const char* data, size_t len) {
    const uint64_t prime = 1099511628211ULL;
    uint64_t hash = 1469598103934665603ULL;

    // process 8-byte chunks
    const uint8_t* p = reinterpret_cast<const uint8_t*>(data);
    const uint8_t* end8 = p + (len & ~size_t(7));
    while (p < end8) {
        uint64_t chunk;
        std::memcpy(&chunk, p, 8);
        for (int i = 0; i < 8; ++i) {
            hash ^= (chunk & 0xFF);
            hash *= prime;
            chunk >>= 8;
        }
        p += 8;
    }
    // tail
    while (p < reinterpret_cast<const uint8_t*>(data + len)) {
        hash ^= static_cast<uint64_t>(*p++);
        hash *= prime;
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
    // pre-compute a tight upper bound for one iteration
    size_t buf_cap = 0;
    for (size_t i = 0; i < n; ++i) {
        // 10 digits uint32 + 1 + 10 digits uint32 + 1 + 5 digits uint16 + 1 + message + 1 newline
        buf_cap += 10 + 1 + 10 + 1 + 5 + 1 + messages[i].size() + 1;
    }
    std::string out;
    out.reserve(buf_cap + 1);

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        out.clear();
        char tmp[32];
        for (size_t i = 0; i < n; ++i) {
            int len = std::snprintf(tmp, sizeof(tmp), "%u|%u|%u|",
                                    timestamps[i], ids[i], levels[i]);
            out.append(tmp, len);
            out.append(messages[i]);
            out.push_back('\n');
        }
        hash = checksum_text(out.data(), out.size());
    }
    return hash;
}