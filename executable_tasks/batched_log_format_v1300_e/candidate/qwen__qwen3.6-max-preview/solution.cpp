#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>
#include <cstddef>

namespace {

inline void update_hash(uint64_t& hash, char c) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
    hash *= 1099511628211ULL;
}

inline void update_hash(uint64_t& hash, const char* s, std::size_t len) {
    for (std::size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(s[i]));
        hash *= 1099511628211ULL;
    }
}

inline int uint_to_str(char* buf, uint32_t val) {
    if (val == 0) { buf[0] = '0'; return 1; }
    int len = 0;
    while (val > 0) {
        buf[len++] = '0' + (val % 10);
        val /= 10;
    }
    for (int i = 0, j = len - 1; i < j; ++i, --j) {
        char t = buf[i]; buf[i] = buf[j]; buf[j] = t;
    }
    return len;
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    uint64_t hash = 0;
    char num_buf[16];
    const std::size_t n = ids.size();
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (std::size_t i = 0; i < n; ++i) {
            int len = uint_to_str(num_buf, timestamps[i]);
            update_hash(hash, num_buf, len);
            update_hash(hash, '|');
            
            len = uint_to_str(num_buf, ids[i]);
            update_hash(hash, num_buf, len);
            update_hash(hash, '|');
            
            len = uint_to_str(num_buf, levels[i]);
            update_hash(hash, num_buf, len);
            update_hash(hash, '|');
            
            const std::string& msg = messages[i];
            update_hash(hash, msg.data(), msg.size());
            update_hash(hash, '\n');
        }
    }
    return hash;
}
