#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline void append_uint32(uint32_t val, uint64_t& hash) {
    char buf[16];
    int len = 0;
    if (val == 0) {
        buf[0] = '0';
        len = 1;
    } else {
        while (val > 0) {
            buf[len++] = '0' + (val % 10);
            val /= 10;
        }
    }
    for (int i = len - 1; i >= 0; --i) {
        hash ^= static_cast<uint64_t>(buf[i]);
        hash *= FNV_PRIME;
    }
}

uint64_t compute_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages) {
    
    uint64_t hash = FNV_OFFSET;
    const size_t n = ids.size();
    for (size_t i = 0; i < n; ++i) {
        append_uint32(timestamps[i], hash);
        hash ^= '|'; hash *= FNV_PRIME;
        append_uint32(ids[i], hash);
        hash ^= '|'; hash *= FNV_PRIME;
        append_uint32(levels[i], hash);
        hash ^= '|'; hash *= FNV_PRIME;
        
        const std::string& msg = messages[i];
        for (unsigned char c : msg) {
            hash ^= c;
            hash *= FNV_PRIME;
        }
        hash ^= '\n'; hash *= FNV_PRIME;
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
    if (iters <= 0) return 0;
    return compute_checksum(timestamps, ids, levels, messages);
}
